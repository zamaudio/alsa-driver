/*
 * 003.h - driver for Digidesign 003Rack
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
 *
 * mostly based on FFADO's efc_cmd.h, which is
 * Copyright (C) 2005-2008 by Pieter Palmers
 *
 */
#ifndef SOUND_FIREWORKS_H_INCLUDED
#define SOUND_FIREWORKS_H_INCLUDED

#include <linux/version.h>
#include <linux/device.h>
#include <linux/firewire.h>
#include <linux/firewire-constants.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/delay.h>
#include <linux/slab.h>

#include <sound/core.h>
#include <sound/initval.h>
#include <sound/control.h>
#include <sound/rawmidi.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/info.h>
#include <sound/tlv.h>

#include "digimagic.h"
#include "packets-buffer.h"
#include "iso-resources.h"
#include "amdtp.h"
#include "cmp.h"
#include "fcp.h"
#include "lib.h"

#define MAX_MIDI_OUTPUTS 2
#define MAX_MIDI_INPUTS 2

#define SND_EFW_MUITIPLIER_MODES 3
#define HWINFO_NAME_SIZE_BYTES 32
#define HWINFO_MAX_CAPS_GROUPS 8

/* for physical metering */
enum snd_efw_channel_type {
	SND_EFW_CHANNEL_TYPE_ANALOG		= 0,
	SND_EFW_CHANNEL_TYPE_SPDIF		= 1,
	SND_EFW_CHANNEL_TYPE_ADAT		= 2,
	SND_EFW_CHANNEL_TYPE_SPDIF_OR_ADAT	= 3,
	SND_EFW_CHANNEL_TYPE_ANALOG_MIRRORING	= 4,
	SND_EFW_CHANNEL_TYPE_HEADPHONES		= 5,
	SND_EFW_CHANNEL_TYPE_I2S		= 6
};
struct snd_efw_phys_group {
	u8 type;	/* enum snd_efw_channel_type */
	u8 count;
} __packed;

struct snd_efw {
	struct snd_card *card;
	struct fw_device *device;
	struct fw_unit *unit;
	int card_index;

	struct mutex mutex;
	spinlock_t lock;

	/* for EFC */
	u32 seqnum;

	/* capabilities */
	unsigned int supported_sampling_rate;
	unsigned int supported_clock_source;
	unsigned int supported_digital_interface;
	unsigned int has_phantom;
	unsigned int has_dsp_mixer;
	unsigned int has_fpga;
	unsigned int aes_ebu_xlr_support;
	unsigned int mirroring_support;
	unsigned int dynaddr_support;

	/* physical metering */
	unsigned int input_group_counts;
	struct snd_efw_phys_group *input_groups;
	unsigned int output_group_counts;
	struct snd_efw_phys_group *output_groups;

	/* meter parameters */
	unsigned int input_meter_counts;
	unsigned int output_meter_counts;

	/* mixer parameters */
	unsigned int mixer_input_channels;
	unsigned int mixer_output_channels;

	/* MIDI parameters */
	unsigned int midi_input_ports;
	unsigned int midi_output_ports;

	/* PCM parameters */
	unsigned int pcm_capture_channels[SND_EFW_MUITIPLIER_MODES];
	unsigned int pcm_playback_channels[SND_EFW_MUITIPLIER_MODES];

	/* notification to control components */
	struct snd_ctl_elem_id *control_id_sampling_rate;

	/* for IEC 61883-1 and -6 streaming */
	struct amdtp_stream receive_stream;
	struct amdtp_stream transmit_stream;
	/* Fireworks has only two plugs */
	struct cmp_connection output_connection;
	struct cmp_connection input_connection;
};

struct snd_efw_hwinfo {
	u32 flags;
	u32 guid_hi;
	u32 guid_lo;
	u32 type;
	u32 version;
	char vendor_name[HWINFO_NAME_SIZE_BYTES];
	char model_name[HWINFO_NAME_SIZE_BYTES];
	u32 supported_clocks;
	u32 nb_1394_playback_channels;
	u32 nb_1394_capture_channels;
	u32 nb_phys_audio_out;
	u32 nb_phys_audio_in;
	u32 nb_out_groups;
	struct snd_efw_phys_group out_groups[HWINFO_MAX_CAPS_GROUPS];
	u32 nb_in_groups;
	struct snd_efw_phys_group in_groups[HWINFO_MAX_CAPS_GROUPS];
	u32 nb_midi_out;
	u32 nb_midi_in;
	u32 max_sample_rate;
	u32 min_sample_rate;
	u32 dsp_version;
	u32 arm_version;
	u32 mixer_playback_channels;
	u32 mixer_capture_channels;
	u32 fpga_version;
	u32 nb_1394_playback_channels_2x;
	u32 nb_1394_capture_channels_2x;
	u32 nb_1394_playback_channels_4x;
	u32 nb_1394_capture_channels_4x;
	u32 reserved[16];
} __packed;

/* for hardware metering */
struct snd_efw_phys_meters {
	u32 status;
	u32 detect_spdif;
	u32 detect_adat;
	u32 reserved0;
	u32 reserved1;
	u32 nb_output_meters;
	u32 nb_input_meters;
	u32 reserved2;
	u32 reserved3;
	u32 values[0];
} __packed;

/* clock source parameters */
enum snd_efw_clock_source {
	SND_EFW_CLOCK_SOURCE_INTERNAL	= 0,
	SND_EFW_CLOCK_SOURCE_SYTMATCH	= 1,
	SND_EFW_CLOCK_SOURCE_WORDCLOCK	= 2,
	SND_EFW_CLOCK_SOURCE_SPDIF	= 3,
	SND_EFW_CLOCK_SOURCE_ADAT_1	= 4,
	SND_EFW_CLOCK_SOURCE_ADAT_2	= 5,
};

/* digital interface parameters */
enum snd_efw_digital_interface {
	SND_EFW_DIGITAL_INTERFACE_SPDIF_COAXIAL	= 0,
	SND_EFW_DIGITAL_INTERFACE_ADAT_COAXIAL	= 1,
	SND_EFW_DIGITAL_INTERFACE_SPDIF_OPTICAL	= 2,
	SND_EFW_DIGITAL_INTERFACE_ADAT_OPTICAL	= 3
};

/* S/PDIF format parameters */
enum snd_efw_iec60958_format {
	SND_EFW_IEC60958_FORMAT_CONSUMER	= 0,
	SND_EFW_IEC60958_FORMAT_PROFESSIONAL	= 1
};

/* for AMDTP stream and CMP */
int snd_efw_sync_streams_init(struct snd_efw *efw);
int snd_efw_sync_streams_start(struct snd_efw *efw);
void snd_efw_sync_streams_stop(struct snd_efw *efw);
void snd_efw_sync_streams_update(struct snd_efw *efw);
void snd_efw_sync_streams_destroy(struct snd_efw *efw);

/* for procfs subsystem */
/* for control component */

/* for midi component */
int snd_efw_create_midi_devices(struct snd_efw *ef);

/* for pcm component */
int snd_efw_create_pcm_devices(struct snd_efw *efw);
void snd_efw_destroy_pcm_devices(struct snd_efw *efw);
int snd_efw_get_multiplier_mode(int sampling_rate);


#endif
