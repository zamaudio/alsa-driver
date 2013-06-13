/*
 * 003.c - driver for 003Rack by Digidesign
 *
 * Copyright (c) 2009-2010 Clemens Ladisch
 * Copyright (c) 2013 Damien Zammit
 * Copyright (c) 2013 Takashi Sakamoto
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

#include "003.h"

MODULE_DESCRIPTION("Digidesign 003 Driver");
MODULE_AUTHOR("Damien Zammit <damien@zamaudio.com>");
MODULE_LICENSE("GPL v2");

static int index[SNDRV_CARDS]   = SNDRV_DEFAULT_IDX;
static char *id[SNDRV_CARDS]    = SNDRV_DEFAULT_STR;
static int enable[SNDRV_CARDS]  = SNDRV_DEFAULT_ENABLE_PNP;

static DEFINE_MUTEX(devices_mutex);
static unsigned int devices_used;

static int snd_efw_get_hardware_info(struct snd_efw_t *efw)
{
	int err = 0;
	int size = 0;
	int i;
	struct efc_hwinfo_t *hwinfo = NULL;

	hwinfo = kmalloc(sizeof(struct efc_hwinfo_t), GFP_KERNEL);
	if (hwinfo == NULL) {
		err = -ENOMEM;
		goto end;
	}
	/* capabilities */
		efw->dynaddr_support = 0;
		efw->mirroring_support = 0;
		efw->aes_ebu_xlr_support = 0;
		efw->has_dsp_mixer = 0;
		efw->has_fpga = 0;
		efw->has_phantom = 0;
		efw->supported_digital_mode = BIT(2) | BIT(3);

	hwinfo->nb_out_groups = 1;
	hwinfo->nb_in_groups = 1;

	/* for input physical metering */
	if (hwinfo->nb_out_groups > 0) {
		size = sizeof(struct snd_efw_phys_group_t);
		efw->output_groups = kmalloc(size, GFP_KERNEL);
		if (efw->output_groups == NULL)
			goto end;

		efw->output_group_counts = 1;
		for (i = 0; i < efw->output_group_counts; i += 1) {
			efw->output_groups[i].type = 0;
			efw->output_groups[i].count = 0;
		}
	}

	/* for output physical metering */
	if (hwinfo->nb_in_groups > 0) {
		size = sizeof(struct snd_efw_phys_group_t);
		efw->input_groups = kmalloc(size, GFP_KERNEL);
		if (efw->input_groups == NULL) {
			if (efw->output_group_counts > 0)
				kfree(efw->output_groups);
			goto end;
		}

		efw->input_group_counts = 1;
		for (i = 0; i < efw->input_group_counts; i += 1) {
			efw->input_groups[i].type = 0;
			efw->input_groups[i].count = 0;
		}
	}

	/* for mixer channels */
	efw->mixer_output_channels = 18;
	efw->mixer_input_channels = 18;

	efw->pcm_capture_channels_sets[0] = 18;
	efw->pcm_capture_channels_sets[1] = 18;
	efw->pcm_capture_channels_sets[2] = 18;
	efw->pcm_playback_channels_sets[0] = 18;
	efw->pcm_playback_channels_sets[1] = 18;
	efw->pcm_playback_channels_sets[2] = 18;

	/* set names */
	strcpy(efw->card->driver, "003 Rack");
	strcpy(efw->card->shortname, "003R");
	snprintf(efw->card->longname, sizeof(efw->card->longname),
		"%s %s at %s, S%d",
		"Digidesign", "003 Rack", 
		dev_name(&efw->unit->device), 100 << efw->device->max_speed);
	strcpy(efw->card->mixername, "003 Rack");

	/* set flag for supported clock source */
	efw->supported_clock_source = 0;
	
	efw->supported_sampling_rate = SNDRV_PCM_RATE_48000;
	
	/* MIDI/PCM inputs and outputs */
	efw->midi_output_count = 2;
	efw->midi_input_count = 1;

end:
	if (hwinfo != NULL)
		kfree(hwinfo);
	return err;
}

static int snd_efw_get_hardware_meters_count(struct snd_efw_t *efw)
{
	efw->input_meter_counts = 0;
	efw->output_meter_counts = 0;
	return 0;
}

static void
snd_efw_update(struct fw_unit *unit)
{
	struct snd_card *card = dev_get_drvdata(&unit->device);
	struct snd_efw_t *efw = card->private_data;

	fcp_bus_reset(efw->unit);

	/* bus reset for isochronous transmit stream */
	if (!(&(efw->receive_stream.conn))) {
		amdtp_out_stream_pcm_abort(&(efw->receive_stream.strm));
		mutex_lock(&efw->mutex);
		snd_efw_stream_stop(efw, &efw->receive_stream);
		mutex_unlock(&efw->mutex);
	}
	amdtp_out_stream_update(&(efw->receive_stream.strm));

	/* bus reset for isochronous receive stream */
	if (!(&(efw->transmit_stream.conn))) {
		amdtp_out_stream_pcm_abort(&(efw->transmit_stream.strm));
		mutex_lock(&efw->mutex);
		snd_efw_stream_stop(efw, &efw->transmit_stream);
		mutex_unlock(&efw->mutex);
	}
	amdtp_out_stream_update(&(efw->transmit_stream.strm));

	return;
}

static bool match_fireworks_device_name(struct fw_unit *unit)
{
	return true;
}

static void snd_efw_card_free(struct snd_card *card)
{
	struct snd_efw_t *efw = card->private_data;

	if (efw->card_index >= 0) {
		mutex_lock(&devices_mutex);
		devices_used &= ~(1 << efw->card_index);
		mutex_unlock(&devices_mutex);
	}

	if (efw->output_group_counts > 0)
		kfree(efw->output_groups);
	if (efw->input_group_counts > 0)
		kfree(efw->input_groups);

	mutex_destroy(&efw->mutex);

	return;
}

static int snd_efw_probe(struct device *dev)
{
	struct fw_unit *unit = fw_unit(dev);
	int card_index;
	struct snd_card *card;
	struct snd_efw_t *efw;
	int err;

	printk("PROBE STARTED\n");
	if (!match_fireworks_device_name(unit))
		return -ENODEV;

	mutex_lock(&devices_mutex);
	for (card_index = 0; card_index < SNDRV_CARDS; ++card_index)
		if (!(devices_used & (1 << card_index)) && enable[card_index])
			break;
	if (card_index >= SNDRV_CARDS) {
		err = -ENOENT;
		goto error_mutex;
	}
	err = snd_card_create(index[card_index], id[card_index],
			THIS_MODULE, sizeof(struct snd_efw_t), &card);
	if (err < 0)
		goto error_mutex;
	card->private_free = snd_efw_card_free;

	efw = card->private_data;
	efw->card = card;
	efw->device = fw_parent_device(unit);
	efw->unit = unit;
	efw->card_index = -1;
	mutex_init(&efw->mutex);
	spin_lock_init(&efw->lock);

	/* identifying */

	/* get hardware information */
	err = snd_efw_get_hardware_info(efw);
	if (err < 0)
		goto error_card;

	/* get the number of hardware meters */
	err = snd_efw_get_hardware_meters_count(efw);
	if (err < 0)
		goto error_card;

	/* create proc interface */
	
	/* create hardware control */

	/* create PCM interface */
	err = snd_efw_create_pcm_devices(efw);
	if (err < 0)
		goto error_card;

	/* create midi interface */
	if (efw->midi_output_count || efw->midi_input_count) {
		err = snd_efw_create_midi_devices(efw);
		if (err < 0)
			goto error_card;
	}

	snd_card_set_dev(card, dev);
	err = snd_card_register(card);
	if (err < 0)
		goto error_card;

	dev_set_drvdata(dev, card);
	devices_used |= 1 << card_index;
	efw->card_index = card_index;

	mutex_unlock(&devices_mutex);
	
	printk("PROBE SUCCEEDED\n");
	return 0;

error_card:
	printk("ERROR CARD\n");
	mutex_unlock(&devices_mutex);
	snd_card_free(card);
	return err;

error_mutex:
	printk("ERROR MUTEX\n");
	mutex_unlock(&devices_mutex);
	return err;
}

static int snd_efw_remove(struct device *dev)
{
	struct snd_card *card = dev_get_drvdata(dev);
	struct snd_efw_t *efw = card->private_data;

	snd_efw_destroy_pcm_devices(efw);

	snd_card_disconnect(card);
	snd_card_free_when_closed(card);

	return 0;
}

#define VENDOR_DIGIDESIGN		0x00a07e
#define MODEL_ID_003RACK		0x00ab00

static const struct ieee1394_device_id snd_efw_id_table[] = {
	{
		.match_flags = IEEE1394_MATCH_VENDOR_ID, 
		.vendor_id = VENDOR_DIGIDESIGN,
	},
	{}
};
MODULE_DEVICE_TABLE(ieee1394, snd_efw_id_table);

static struct fw_driver snd_efw_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "snd-003",
		.bus = &fw_bus_type,
		.probe = snd_efw_probe,
		.remove = snd_efw_remove,
	},
	.update = snd_efw_update,
	.id_table = snd_efw_id_table,
};

static int __init snd_efw_init(void)
{
	return driver_register(&snd_efw_driver.driver);
}

static void __exit snd_efw_exit(void)
{
	driver_unregister(&snd_efw_driver.driver);
	mutex_destroy(&devices_mutex);
}

module_init(snd_efw_init);
module_exit(snd_efw_exit);
