/*
 * fireworks_proc.c - driver for Firewire devices from Echo Digital Audio
 *
 * Copyright (c) 2009-2010 Clemens Ladisch
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

static void
snd_efw_proc_read_clock(struct snd_info_entry *entry, struct snd_info_buffer *buffer)
{
	struct snd_efw_t *efw = entry->private_data;
	enum snd_efw_clock_source_t clock_source;
	int sampling_rate;

	if (snd_efw_command_get_clock_source(efw, &clock_source) < 0)
		goto end;

	if (snd_efw_command_get_sampling_rate(efw, &sampling_rate) < 0)
		goto end;

	snd_iprintf(buffer, "Clock Source: Internal\n");
	snd_iprintf(buffer, "Sampling Rate: 48000Hz\n");

end:
	return;
}

static void
snd_efw_proc_read_phys_meters(struct snd_info_entry *entry, struct snd_info_buffer *buffer)
{
	u32 *meters = NULL;

	char const *descs[] = {"Analog", "S/PDIF", "ADAT", "S/PDIF or ADAT",
				"Analog Mirroring", "Headphones", "I2S"};

	struct snd_efw_t *efw = entry->private_data;
	int i, g, c;
	int count = efw->input_meter_counts + efw->output_meter_counts;

	meters = kmalloc(count * sizeof(u32), GFP_KERNEL);
	if (meters == NULL)
		goto end;

	if (snd_efw_command_get_phys_meters(efw, count, meters) < 0) {
		kfree(meters);
		goto end;
	}

	snd_iprintf(buffer, "Physical Meters:\n");

	if (efw->pcm_playback_channels > 0) {
		snd_iprintf(buffer, " %d Outputs:\n", efw->pcm_playback_channels);

		g = 0;
		c = 0;
		for (i = 0; i < efw->pcm_playback_channels; i += 1) {
			if (c == efw->output_groups[g].count) {
				g += 1;
				c = 0;
			}
			snd_iprintf(buffer, "  %s [%d]:  %d\n",
				descs[efw->output_groups[g].type], c, meters[i]);
			c += 1;
		}
	}

	if (efw->pcm_capture_channels > 0) {
		snd_iprintf(buffer, " %d Inputs:\n", efw->pcm_capture_channels);

		g = 0;
		c = 0;
		for (i = 0; i < efw->pcm_capture_channels; i += 1) {
			if (c == efw->input_groups[g].count) {
				g += 1;
				c = 0;
			}
			snd_iprintf(buffer, "  %s [%d]: %d\n",
				descs[efw->input_groups[g].type], c, meters[efw->mixer_output_channels + i]);
			c += 1;
		}
	}

end:
	if (meters != NULL)
		kfree(meters);
	return;
}

static void
print_mixer_values(struct snd_efw_t *efw, struct snd_info_buffer *buffer,
	int(*func)(struct snd_efw_t *, enum snd_efw_mixer_cmd_t, int, int *),
	int channel)
{
	int value;

	if (func(efw, SND_EFW_MIXER_GET_GAIN, channel, &value) < 0)
		snd_iprintf(buffer, "*\t");
	else
		snd_iprintf(buffer, "%d\t", value);

	if (func(efw, SND_EFW_MIXER_GET_MUTE, channel, &value) < 0)
		snd_iprintf(buffer, "*\t");
	else
		snd_iprintf(buffer, "%d\t", value);

	if (func(efw, SND_EFW_MIXER_GET_SOLO, channel, &value) < 0)
		snd_iprintf(buffer, "*\t");
	else
		snd_iprintf(buffer, "%d\t", value);

	if (func(efw, SND_EFW_MIXER_GET_PAN, channel, &value) < 0)
		snd_iprintf(buffer, "*\t");
	else
		snd_iprintf(buffer, "%d\t", value);

	if (func(efw, SND_EFW_MIXER_GET_NOMINAL, channel, &value) < 0)
		snd_iprintf(buffer, "*\n");
	else
		snd_iprintf(buffer, "%d\n", value);

	return;
}

static void
snd_efw_proc_read_mixer(struct snd_info_entry *entry, struct snd_info_buffer *buffer)
{
	struct snd_efw_t *efw = entry->private_data;
	int c;

	snd_iprintf(buffer, "\n\t\tGain\t\tMute\tSolo\tPan\tNominal\n");
	for (c = 0; c < efw->pcm_playback_channels; c += 1) {
		snd_iprintf(buffer, "PCM Play[%d]:\t", c);
		print_mixer_values(efw, buffer, snd_efw_command_playback, c);
	}

	snd_iprintf(buffer, "\n\t\tGain\t\tMute\tSolo\tPan\tNominal\n");
	for (c = 0; c < efw->output_meter_counts; c += 1) {
		snd_iprintf(buffer, "Phys Out[%d]:\t", c);
		print_mixer_values(efw, buffer, snd_efw_command_phys_out, c);
	}

	snd_iprintf(buffer, "\n\t\tGain\t\tMute\tSolo\tPan\tNominal\n");
	for (c = 0; c < efw->output_meter_counts; c += 1) {
		snd_iprintf(buffer, "PCM Cap[%d]:\t", c);
		print_mixer_values(efw, buffer, snd_efw_command_capture, c);
	}

	snd_iprintf(buffer, "\n\t\tGain\t\tMute\tSolo\tPan\tNominal\n");
	for (c = 0; c < efw->output_meter_counts; c += 1) {
		snd_iprintf(buffer, "Phys In[%d]:\t", c);
		print_mixer_values(efw, buffer, snd_efw_command_phys_in, c);
	}

	return;
}

static void
snd_efw_proc_read_monitor(struct snd_info_entry *entry, struct snd_info_buffer *buffer)
{
	struct snd_efw_t *efw = entry->private_data;
	int value;
	int i, o;

	for (i = 0; i < efw->mixer_input_channels; i += 1 ) {
		snd_iprintf(buffer, "\n\t\tGain\t\tMute\tSolo\tPan\n");
		for (o = 0; o < efw->mixer_output_channels; o += 1) {
			snd_iprintf(buffer, "IN[%d]:OUT[%d]:\t", i, o);
			if (snd_efw_command_monitor(efw, SND_EFW_MIXER_GET_GAIN,
							i, o, &value) < 0)
				snd_iprintf(buffer, "*\t");
			else
				snd_iprintf(buffer, "%d\t", value);

			if (snd_efw_command_monitor(efw, SND_EFW_MIXER_GET_MUTE,
							i, o, &value) < 0)
				snd_iprintf(buffer, "*\t");
			else
				snd_iprintf(buffer, "%d\t", value);

			if (snd_efw_command_monitor(efw, SND_EFW_MIXER_GET_SOLO,
							i, o, &value) < 0)
				snd_iprintf(buffer, "*\t");
			else
				snd_iprintf(buffer, "%d\t", value);

			if (snd_efw_command_monitor(efw, SND_EFW_MIXER_GET_PAN,
							i, o, &value) < 0)
				snd_iprintf(buffer, "*\n");
			else
				snd_iprintf(buffer, "%d\n", value);

		}
	}

	return;
}


void snd_efw_proc_init(struct snd_efw_t *efw)
{
	struct snd_info_entry *entry;

	if(!snd_card_proc_new(efw->card, "#clock", &entry))
		snd_info_set_text_ops(entry, efw, snd_efw_proc_read_clock);
	if(!snd_card_proc_new(efw->card, "#meters", &entry))
		snd_info_set_text_ops(entry, efw, snd_efw_proc_read_phys_meters);
	if(!snd_card_proc_new(efw->card, "#mixer", &entry))
		snd_info_set_text_ops(entry, efw, snd_efw_proc_read_mixer);
	if(!snd_card_proc_new(efw->card, "#monitor", &entry))
		snd_info_set_text_ops(entry, efw, snd_efw_proc_read_monitor);
	return;
}
