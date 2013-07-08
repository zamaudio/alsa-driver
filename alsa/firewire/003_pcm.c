/*
 * 003_pcm.c - pcm driver for Digidesign 003Rack
 *
 * Copyright (c) 2009-2010 Clemens Ladisch
 * Copyright (c) 2013 Takashi Sakamoto
 * Copyright (c) 2013 Damien Zammit
 *
 *
 * This driver is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.
 *
 * This driver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this driver; if not, see <http://www.gnu.org/licenses/>.
 */
#include "003_lowlevel.h"
#include <linux/spinlock.h>
#include <linux/wait.h>

static DEFINE_MUTEX(devices_mutex);

static int digi_hw_lock(struct snd_efw *digi)
{
        int err;

        spin_lock_irq(&digi->lockhw);

        if (digi->dev_lock_count == 0) {
                digi->dev_lock_count = -1;
                err = 0;
        } else {
                err = -EBUSY;
        }

        spin_unlock_irq(&digi->lockhw);

        return err;
}

static int digi_hw_unlock(struct snd_efw *digi)
{
        int err;

        spin_lock_irq(&digi->lockhw);
        
        if (digi->dev_lock_count == -1) {
                digi->dev_lock_count = 0;
                err = 0;
        } else {
                err = -EBADFD;
        }

        spin_unlock_irq(&digi->lockhw);

        return err;
}       

static void digi_lock_changed(struct snd_efw *digi)
{
        digi->dev_lock_changed = true;
        wake_up(&digi->hwdep_wait);
}

static int digi_try_lock(struct snd_efw *digi)
{
        int err;

        spin_lock_irq(&digi->lockhw);

        if (digi->dev_lock_count < 0) {
                err = -EBUSY;
                goto out;
        }

        if (digi->dev_lock_count++ == 0)
                digi_lock_changed(digi);
        err = 0;

out:
        spin_unlock_irq(&digi->lockhw);

        return err;
}

static void digi_unlock(struct snd_efw *digi)
{
        spin_lock_irq(&digi->lockhw);

        if (WARN_ON(digi->dev_lock_count <= 0))
                goto out;

        if (--digi->dev_lock_count == 0)
                digi_lock_changed(digi);

out:
        spin_unlock_irq(&digi->lockhw);
}

static int
pcm_init_hw_params(struct snd_efw *efw,
		   struct snd_pcm_substream *substream)
{
	static const struct snd_pcm_hardware hardware = {
		.info = SNDRV_PCM_INFO_MMAP |
			SNDRV_PCM_INFO_BATCH |
			SNDRV_PCM_INFO_INTERLEAVED |
			SNDRV_PCM_INFO_SYNC_START |
			SNDRV_PCM_INFO_FIFO_IN_FRAMES |
			/* for Open Sound System compatibility */
			SNDRV_PCM_INFO_MMAP_VALID |
			SNDRV_PCM_INFO_BLOCK_TRANSFER,
		.formats = SNDRV_PCM_FMTBIT_S32,
		.channels_min = 18,
		.channels_max = 18,
		.buffer_bytes_max = 1024 * 1024 * 1024,
		.period_bytes_min = 256,
		.period_bytes_max = 1024 * 1024 * 1024 / 2,
		.periods_min = 2,
		.periods_max = 32,
		.fifo_size = 0,
	};
	
	int err, i;
	
	mutex_lock(&efw->mutexhw);

	printk("START HW_PARAMS\n");
		
        err = digi_hw_lock(efw);
        if (err < 0)
                goto error1;
	printk("HW LOCK COMPLETE\n");
       	 
        //err = digi_hw_unlock(efw);
        //if (err < 0)
        //        goto error1;
	//printk("HW UNLOCK COMPLETE\n");
	
	//err = digi_try_lock(efw);
        //if (err < 0)
        //        goto error2;
	//printk("TRY LOCK COMPLETE\n");

	substream->runtime->hw = hardware;
	printk("HW COMPLETE\n");

	substream->runtime->delay = substream->runtime->hw.fifo_size;

        //substream->runtime->hw.rates = 0;
        substream->runtime->hw.rates |= snd_pcm_rate_to_rate_bit(48000);
        snd_pcm_limit_hw_rates(substream->runtime);

	substream->runtime->hw.formats = SNDRV_PCM_FMTBIT_S32;
	substream->runtime->hw.channels_min = 18;
	substream->runtime->hw.channels_max = 18;

	/* AM824 in IEC 61883-6 can deliver 24bit data */
	err = snd_pcm_hw_constraint_msbits(substream->runtime, 0, 32, 24);
	if (err < 0)
		goto end;

/*
	err = snd_pcm_hw_constraint_step(substream->runtime, 0,
				SNDRV_PCM_HW_PARAM_PERIOD_BYTES, 32);
	if (err < 0)
		goto end;
	err = snd_pcm_hw_constraint_step(substream->runtime, 0,
				SNDRV_PCM_HW_PARAM_BUFFER_BYTES, 32);
	if (err < 0)
		goto end;
*/

	/* time for period constraint */
	err = snd_pcm_hw_constraint_minmax(substream->runtime,
					SNDRV_PCM_HW_PARAM_PERIOD_TIME,
					500, UINT_MAX);
	if (err < 0)
		goto end;

	err = 0;
	printk("END HW_PARAMS");
end:
	//digi_unlock(efw);	
error2:
        digi_hw_unlock(efw);
error1:
	mutex_unlock(&efw->mutexhw);
        return err;
}

static int
pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_efw *efw = substream->private_data;
	int err;
	
	mutex_lock(&devices_mutex);

	printk("START PCM_OPEN\n");
	
	mutex_init(&efw->mutexhw);
	spin_lock_init(&efw->lockhw);

	/* common hardware information */
	err = pcm_init_hw_params(efw, substream);
	if (err < 0)
		goto end;

	printk("DONE HW_PARAMS\n");
	
	substream->runtime->hw.channels_min = 18;
	substream->runtime->hw.channels_max = 18;
	substream->runtime->hw.rate_min = 48000;
	substream->runtime->hw.rate_max = 48000;

	printk("PRE RACK_INIT\n");
	rack_init(efw);
	printk("DONE RACK_INIT\n");

	snd_pcm_set_sync(substream);
	printk("DONE SET_SYNC\n");
	
	return 0;

end:
	mutex_destroy(&efw->mutexhw);
	mutex_unlock(&devices_mutex);
	printk("ERROR PCM_OPEN: HW_PARAMS\n");
	return err;
}

static int
pcm_close(struct snd_pcm_substream *substream)
{
	return 0;
}

static int
pcm_hw_params(struct snd_pcm_substream *substream,
	      struct snd_pcm_hw_params *hw_params)
{
	struct snd_efw *efw = substream->private_data;
	struct amdtp_stream *stream, *opposite;
	int err;

	/* keep PCM ring buffer */
	err = snd_pcm_lib_alloc_vmalloc_buffer(substream,
				params_buffer_bytes(hw_params));
	if (err < 0)
		goto end;

	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
		stream = &efw->receive_stream;
		opposite = &efw->transmit_stream;
	} else {
		stream = &efw->transmit_stream;
		opposite = &efw->receive_stream;
	}

	/* decide transfer function */
	amdtp_stream_set_pcm_format(stream, params_format(hw_params));

	/*
	 * when opposite PCM stream is not running, stop streams. Then any
	 * MIDI streams stop temporarily. Then set requested sampling rate
	 * and restart.
	 * When oppiste PCM stream is running, don't change sampling rate and
	 * don't need to restart.
	 */
	if (!amdtp_stream_pcm_running(opposite)) {
		/* confirm to stop because MIDI may still use it */
		snd_efw_sync_streams_stop(efw);

		/* restart */
		err = snd_efw_sync_streams_start(efw);
	}

end:
	return err;
}

static int
pcm_hw_free(struct snd_pcm_substream *substream)
{
	struct snd_efw *efw = substream->private_data;

	/* if no PCM/MIDI streams are running, then stop streams */
	/* TODO: I wonder why but transmit midi is running??? */
	if (!amdtp_stream_pcm_running(&efw->transmit_stream) &&
	    !amdtp_stream_pcm_running(&efw->receive_stream) &&
	    !amdtp_stream_midi_running(&efw->transmit_stream) &&
	    !amdtp_stream_midi_running(&efw->receive_stream))
		snd_efw_sync_streams_stop(efw);

	return snd_pcm_lib_free_vmalloc_buffer(substream);
}

/* It's OK just to consider this pcm substream. */
static int
pcm_prepare(struct snd_pcm_substream *substream)
{
	struct snd_efw *efw = substream->private_data;
	struct amdtp_stream *stream;
	int err = 0;

	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
		stream = &efw->receive_stream;
	else
		stream = &efw->transmit_stream;

	/* wait for stream run */
	if (!amdtp_stream_wait_run(stream)) {
		err = -EIO;
		goto end;
	}

	/* initialize buffer pointer */
	amdtp_stream_pcm_prepare(stream);

end:
	return err;
}

/* It's OK just to consider this pcm substream. */
static int
pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct snd_efw *efw = substream->private_data;
	struct amdtp_stream *stream;

	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
		stream = &efw->receive_stream;
	else
		stream = &efw->transmit_stream;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		amdtp_stream_pcm_trigger(stream, substream);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		amdtp_stream_pcm_trigger(stream, NULL);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/* It's OK just to consider this pcm substream. */
static snd_pcm_uframes_t
pcm_pointer(struct snd_pcm_substream *substream)
{
	struct snd_efw *efw = substream->private_data;

	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
		return amdtp_stream_pcm_pointer(&efw->receive_stream);
	else
		return amdtp_stream_pcm_pointer(&efw->transmit_stream);
}

static struct snd_pcm_ops pcm_playback_ops = {
	.open		= pcm_open,
	.close		= pcm_close,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_params	= pcm_hw_params,
	.hw_free	= pcm_hw_free,
	.prepare	= pcm_prepare,
	.trigger	= pcm_trigger,
	.pointer	= pcm_pointer,
	.page		= snd_pcm_lib_get_vmalloc_page,
	.mmap		= snd_pcm_lib_mmap_vmalloc,
};

static struct snd_pcm_ops pcm_capture_ops = {
	.open		= pcm_open,
	.close		= pcm_close,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_params	= pcm_hw_params,
	.hw_free	= pcm_hw_free,
	.prepare	= pcm_prepare,
	.trigger	= pcm_trigger,
	.pointer	= pcm_pointer,
	.page		= snd_pcm_lib_get_vmalloc_page,
//	.mmap		= snd_pcm_lib_mmap_vmalloc,  //dz
};

int snd_efw_create_pcm_devices(struct snd_efw *efw)
{
	struct snd_pcm *pcm;
	int err;

	err = snd_pcm_new(efw->card, efw->card->driver, 0, 1, 1, &pcm);
	if (err < 0)
		goto end;

	pcm->private_data = efw;
	snprintf(pcm->name, sizeof(pcm->name), "%s PCM", efw->card->shortname);
	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &pcm_playback_ops);
	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &pcm_capture_ops);

	snd_efw_sync_streams_init(efw);
end:
	return err;
}

void snd_efw_destroy_pcm_devices(struct snd_efw *efw)
{
	snd_efw_sync_streams_destroy(efw);
	return;
}
