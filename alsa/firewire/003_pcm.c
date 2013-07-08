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

/*
 * NOTE:
 * Fireworks changes its PCM channels according to its sampling rate.
 * There are three modes. Here "capture" or "playback" is appplied to XXX.
 *  0:  32.0- 48.0 kHz then snd_efw_hwinfo.nb_1394_XXX_channels    applied
 *  1:  88.2- 96.0 kHz then snd_efw_hwinfo.nb_1394_XXX_channels_2x applied
 *  2: 176.4-192.0 kHz then snd_efw_hwinfo.nb_1394_XXX_channels_4x applied
 *
 * Then the number of PCM channels for analog input and output are always fixed
 * but the number of PCM channels for digital input and output are differed.
 *
 * Additionally, according to "AudioFire Owner's Manual Version 2.2",
 * the number of PCM channels for digital input has more restriction
 * depending on which digital interface is selected.
 *  - S/PDIF coaxial and optical	: use input 1-2
 *  - ADAT optical at 32.0-48.0 kHz	: use input 1-8
 *  - ADAT optical at 88.2-96.0 kHz	: use input 1-4 (S/MUX format)
 * Even if these restriction is applied, the number of channels in AMDTP stream
 * is decided according to above 0/1/2 modes. The needless data is filled with
 * zero.
 *
 * Currently this module doesn't support the latter.
 */
static unsigned int freq_table[] = {
	/* multiplier mode 0 */
	[0] = 48000,
	[1] = 48000,
	[2] = 48000,
	/* multiplier mode 1 */
	[3] = 96000,
	[4] = 96000,
	/* multiplier mode 2 */
	[5] = 96000,
	[6] = 96000,
};

static inline int
get_multiplier_mode_with_index(int index)
{
	return ((int)index - 1) / 2;
}

int snd_efw_get_multiplier_mode(int sampling_rate)
{
	int i;
	for (i = 0; i < sizeof(freq_table); i += 1)
		if (freq_table[i] == sampling_rate)
			return get_multiplier_mode_with_index(i);

	return -1;
}

static int
hw_rule_rate(struct snd_pcm_hw_params *params,
	     struct snd_pcm_hw_rule *rule,
	     struct snd_efw *efw, unsigned int *channels)
{
	struct snd_interval *r =
		hw_param_interval(params, SNDRV_PCM_HW_PARAM_RATE);
	const struct snd_interval *c =
		hw_param_interval_c(params, SNDRV_PCM_HW_PARAM_CHANNELS);
	struct snd_interval t = {
		.min = UINT_MAX, .max = 0, .integer = 1
	};
	unsigned int rate_bit;
	int mode, i;

	for (i = 0; i < ARRAY_SIZE(freq_table); i += 1) {
		/* skip unsupported sampling rate */
		rate_bit = snd_pcm_rate_to_rate_bit(freq_table[i]);
		if (!(efw->supported_sampling_rate & rate_bit))
			continue;

		mode = get_multiplier_mode_with_index(i);
		if (!snd_interval_test(c, channels[mode]))
			continue;

		t.min = min(t.min, freq_table[i]);
		t.max = max(t.max, freq_table[i]);

	}

	return snd_interval_refine(r, &t);
}

static int
hw_rule_channels(struct snd_pcm_hw_params *params,
		 struct snd_pcm_hw_rule *rule,
		 struct snd_efw *efw, unsigned int *channels)
{
	struct snd_interval *c =
		hw_param_interval(params, SNDRV_PCM_HW_PARAM_CHANNELS);
	const struct snd_interval *r =
		hw_param_interval_c(params, SNDRV_PCM_HW_PARAM_RATE);
	struct snd_interval t = {
		.min = UINT_MAX, .max = 0, .integer = 1
	};

	unsigned int rate_bit;
	int mode, i;

	for (i = 0; i < ARRAY_SIZE(freq_table); i += 1) {
		/* skip unsupported sampling rate */
		rate_bit = snd_pcm_rate_to_rate_bit(freq_table[i]);
		if (!(efw->supported_sampling_rate & rate_bit))
			continue;

		mode = get_multiplier_mode_with_index(i);
		if (!snd_interval_test(r, freq_table[i]))
			continue;

		t.min = min(t.min, channels[mode]);
		t.max = max(t.max, channels[mode]);

	}

	return snd_interval_refine(c, &t);
}

static int
hw_rule_capture_rate(struct snd_pcm_hw_params *params,
		     struct snd_pcm_hw_rule *rule)
{
	struct snd_efw *efw = rule->private;
	return hw_rule_rate(params, rule, efw,
				efw->pcm_capture_channels);
}

static int
hw_rule_playback_rate(struct snd_pcm_hw_params *params,
		      struct snd_pcm_hw_rule *rule)
{
	struct snd_efw *efw = rule->private;
	return hw_rule_rate(params, rule, efw,
				efw->pcm_playback_channels);
}

static int
hw_rule_capture_channels(struct snd_pcm_hw_params *params,
			 struct snd_pcm_hw_rule *rule)
{
	struct snd_efw *efw = rule->private;
	return hw_rule_channels(params, rule, efw,
				efw->pcm_capture_channels);
}

static int
hw_rule_playback_channels(struct snd_pcm_hw_params *params,
			  struct snd_pcm_hw_rule *rule)
{
	struct snd_efw *efw = rule->private;
	return hw_rule_channels(params, rule, efw,
				efw->pcm_playback_channels);
}

static int
pcm_init_hw_params(struct snd_efw *efw,
		   struct snd_pcm_substream *substream)
{
	unsigned int *pcm_channels;
	unsigned int rate_bit;
	int mode, i;
	int err;

	struct snd_pcm_hardware hardware = {
		.info = SNDRV_PCM_INFO_MMAP |
			SNDRV_PCM_INFO_BATCH |
			SNDRV_PCM_INFO_INTERLEAVED |
			SNDRV_PCM_INFO_SYNC_START |
			SNDRV_PCM_INFO_FIFO_IN_FRAMES |
			/* for Open Sound System compatibility */
			SNDRV_PCM_INFO_MMAP_VALID |
			SNDRV_PCM_INFO_BLOCK_TRANSFER,
		.rates = SNDRV_PCM_RATE_48000,
		.rate_min = 48000,
		.rate_max = 48000,
		.channels_min = 19,
		.channels_max = 19,
		.buffer_bytes_max = 1024 * 1024 * 1024,
		.period_bytes_min = 256,
		.period_bytes_max = 1024 * 1024 * 1024 / 2,
		.periods_min = 2,
		.periods_max = 32,
		.fifo_size = 0,
	};

	substream->runtime->hw = hardware;
	substream->runtime->delay = substream->runtime->hw.fifo_size;

	/* add rule between channels and sampling rate */
	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
		substream->runtime->hw.formats = SNDRV_PCM_FMTBIT_S32_LE;
		snd_pcm_hw_rule_add(substream->runtime, 0,
				SNDRV_PCM_HW_PARAM_CHANNELS,
				hw_rule_capture_channels, efw,
				SNDRV_PCM_HW_PARAM_RATE, -1);
		snd_pcm_hw_rule_add(substream->runtime, 0,
				SNDRV_PCM_HW_PARAM_RATE,
				hw_rule_capture_rate, efw,
				SNDRV_PCM_HW_PARAM_CHANNELS, -1);
		pcm_channels = efw->pcm_capture_channels;
	} else {
		substream->runtime->hw.formats = AMDTP_OUT_PCM_FORMAT_BITS;
		snd_pcm_hw_rule_add(substream->runtime, 0,
				SNDRV_PCM_HW_PARAM_CHANNELS,
				hw_rule_playback_channels, efw,
				SNDRV_PCM_HW_PARAM_RATE, -1);
		snd_pcm_hw_rule_add(substream->runtime, 0,
				SNDRV_PCM_HW_PARAM_RATE,
				hw_rule_playback_rate, efw,
				SNDRV_PCM_HW_PARAM_CHANNELS, -1);
		pcm_channels = efw->pcm_playback_channels;
	}

	/* preparing min/max sampling rate */
	snd_pcm_limit_hw_rates(substream->runtime);

	/* preparing the number of channels */
	for (i = 0; i < ARRAY_SIZE(freq_table); i += 1) {
		/* skip unsupported sampling rate */
		rate_bit = snd_pcm_rate_to_rate_bit(freq_table[i]);
		if (!(efw->supported_sampling_rate & rate_bit))
			continue;

		mode = get_multiplier_mode_with_index(i);
		if (pcm_channels[mode] == 0)
			continue;
		substream->runtime->hw.channels_min =
			min(substream->runtime->hw.channels_min,
				pcm_channels[mode]);
		substream->runtime->hw.channels_max =
			max(substream->runtime->hw.channels_max,
				pcm_channels[mode]);
	}

	/* AM824 in IEC 61883-6 can deliver 24bit data */
	err = snd_pcm_hw_constraint_msbits(substream->runtime, 0, 32, 24);
	if (err < 0)
		goto end;

	/* TODO: format of PCM samples is 16bit or 24bit inner 32bit */
	err = snd_pcm_hw_constraint_step(substream->runtime, 0,
				SNDRV_PCM_HW_PARAM_PERIOD_BYTES, 32);
	if (err < 0)
		goto end;
	/* TODO: */
	err = snd_pcm_hw_constraint_step(substream->runtime, 0,
				SNDRV_PCM_HW_PARAM_BUFFER_BYTES, 32);
	if (err < 0)
		goto end;

	/* time for period constraint */
	err = snd_pcm_hw_constraint_minmax(substream->runtime,
					SNDRV_PCM_HW_PARAM_PERIOD_TIME,
					500, UINT_MAX);
	if (err < 0)
		goto end;

	err = 0;

end:
	return err;
}

static int
pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_efw *efw = substream->private_data;
	int err;

	/* common hardware information */
	err = pcm_init_hw_params(efw, substream);
	if (err < 0)
		goto end;

	efw->pcm_capture_channels[0] = 19;
	efw->pcm_playback_channels[0] = 19;

	substream->runtime->hw.channels_min = efw->pcm_capture_channels[0];
	substream->runtime->hw.channels_max = efw->pcm_capture_channels[0];
	substream->runtime->hw.rate_min = 48000;
	substream->runtime->hw.rate_max = 48000;

	rack_init(efw);

	snd_pcm_set_sync(substream);
	
	return 0;

end:
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
