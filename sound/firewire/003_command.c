/*
 * fireworks_command.c - driver for Firewire devices from Echo Digital Audio
 *
 * Copyright (c) 2013 Takashi Sakamoto <o-takashi@sakmocchi.jp>
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
 * mostly based on FFADO's souce, which is
 * Copyright (C) 2005-2008 by Pieter Palmers
 *
 */

/*
 * According to AV/C command specification, vendors can define their own command.
 *
 * 1394 Trade Association's AV/C Digital Interface Command Set General Specification 4.2
 * (September 1, 2004)
 *  9.6 VENDOR-DEPENDENT commands
 *  (to page 55)
 *   opcode:		0x00
 *   operand[0-2]:	company ID
 *   operand[3-]:	vendor dependent data
 *
 * Echo's Fireworks (trademark? I have no idea...) utilize this specification.
 * We call it as 'Echo Fireworks Commands' (a.k.a EFC).
 *
 * In EFC,
 *  Company ID is always 0x00.
 *   operand[0]:	0x00
 *   operand[1]:	0x00
 *   operand[2]:	0x00
 *
 *  Two blank operands exists in the beginning of the 'vendor dependent data'
 *  This seems to be for data alignment of 32 bit.
 *   operand[3]:	0x00
 *   operand[4]:	0x00
 *
 *  Following to the two operands, EFC substance exists.
 *   At first, 6 data exist. we call these data as 'EFC fields'.
 *   Following to the 6 data, parameters for each commands exists.
 *   Almost of parameters are 32 bit. But exception exists according to command.
 *    data[0]:	length of EFC substance.
 *    data[1]:	EFC version
 *    data[2]:	sequence number. this is incremented in return value
 *    data[3]:	EFC category. If greater than 1,
 *			 EFC_CAT_HWINFO return extended fields.
 *    data[4]:	EFC command
 *    data[5]:	EFC return value against EFC request
 *    data[6-]:	parameters
 *
 * As a result, Echo's Fireworks doesn't need generic command sets.
 */

#include "003.h"

#define POLLED_MAX_NB_METERS 100

/*
 * AV/C parameters for Vendor-Dependent command
 */
#define AVC_CTS			0x00
#define AVC_CTYPE		0x00
#define AVC_SUBUNIT_TYPE	0x1F
#define AVC_SUBUNIT_ID		0x07
#define AVC_OPCODE		0x00
#define AVC_COMPANY_ID		0x00

struct avc_fields_t {
	unsigned short cts:4;
	unsigned short ctype:4;
	unsigned short subunit_type:5;
	unsigned short subunit_id:3;
	unsigned short opcode:8;
	unsigned long company_id:24;
};

/*
 * EFC command
 * quadlet parameters following to this fields.
 */
struct efc_fields_t {
	u32 length;
	u32 version;
	u32 seqnum;
	u32 category;
	u32 command;
	u32 retval;
};

/* command categories */
enum efc_category_t {
	EFC_CAT_HWINFO			= 0,
	EFC_CAT_FLASH			= 1,
	EFC_CAT_HWCTL			= 3,
	EFC_CAT_MIXER_PHYS_OUT		= 4,
	EFC_CAT_MIXER_PHYS_IN		= 5,
	EFC_CAT_MIXER_PLAYBACK		= 6,
	EFC_CAT_MIXER_CAPTURE		= 7,
	EFC_CAT_MIXER_MONITOR		= 8,
	EFC_CAT_IOCONF			= 9,
	/* unused */
	EFC_CAT_TRANSPORT		= 2,
};

/* hardware info category commands */
enum efc_cmd_hwinfo_t {
	EFC_CMD_HWINFO_GET_CAPS			= 0,
	EFC_CMD_HWINFO_GET_POLLED		= 1,
	EFC_CMD_HWINFO_SET_EFR_ADDRESS		= 2,
	EFC_CMD_HWINFO_READ_SESSION_BLOCK	= 3,
	EFC_CMD_HWINFO_GET_DEBUG_INFO		= 4,
	EFC_CMD_HWINFO_SET_DEBUG_TRACKING	= 5
};

/* flash category commands */
enum efc_cmd_flash_t {
	EFC_CMD_FLASH_ERASE		= 0,
	EFC_CMD_FLASH_READ		= 1,
	EFC_CMD_FLASH_WRITE		= 2,
	EFC_CMD_FLASH_GET_STATUS	= 3,
	EFC_CMD_FLASH_GET_SESSION_BASE	= 4,
	EFC_CMD_FLASH_LOCK		= 5
};

/* hardware control category commands */
enum efc_cmd_hwctl_t {
	EFC_CMD_HWCTL_SET_CLOCK		= 0,
	EFC_CMD_HWCTL_GET_CLOCK		= 1,
	EFC_CMD_HWCTL_BSX_HANDSHAKE	= 2,
	EFC_CMD_HWCTL_CHANGE_FLAGS	= 3,
	EFC_CMD_HWCTL_GET_FLAGS		= 4,
	EFC_CMD_HWCTL_IDENTIFY		= 5,
	EFC_CMD_HWCTL_RECONNECT_PHY	= 6
};
/* for flags */
#define EFC_HWCTL_FLAG_MIXER_UNUSABLE	0x00
#define EFC_HWCTL_FLAG_MIXER_USABLE	0x01
#define EFC_HWCTL_FLAG_DIGITAL_PRO	0x02
#define EFC_HWCTL_FLAG_DIGITAL_RAW	0x04

/* I/O config category commands */
enum efc_cmd_ioconf_t {
	EFC_CMD_IOCONF_SET_MIRROR	= 0,
	EFC_CMD_IOCONF_GET_MIRROR	= 1,
	EFC_CMD_IOCONF_SET_DIGITAL_MODE	= 2,
	EFC_CMD_IOCONF_GET_DIGITAL_MODE	= 3,
	EFC_CMD_IOCONF_SET_PHANTOM	= 4,
	EFC_CMD_IOCONF_GET_PHANTOM	= 5,
	EFC_CMD_IOCONF_SET_ISOC_MAP	= 6,
	EFC_CMD_IOCONF_GET_ISOC_MAP	= 7,
};

/* for clock source and sampling rate */
struct efc_clock_t {
	u32 source;
	u32 sampling_rate;
	u32 index;
};

/* for hardware metering s*/
struct efc_polled_values_t {
	u32 status;
	u32 detect_spdif;
	u32 detect_adat;
	u32 reserved3;
	u32 reserved4;
	u32 nb_output_meters;
	u32 nb_input_meters;
	u32 reserved5;
	u32 reserved6;
};

/* channel types */
/* MIDI */
/* Guitar */
/* Piezo pickup */
/* Guitar string */
/* hardware control flags */

/* return values */
enum efc_retval_t {
	EFC_RETVAL_OK			= 0,
	EFC_RETVAL_BAD			= 1,
	EFC_RETVAL_BAD_COMMAND		= 2,
	EFC_RETVAL_COMM_ERR		= 3,
	EFC_RETVAL_BAD_QUAD_COUNT	= 4,
	EFC_RETVAL_UNSUPPORTED		= 5,
	EFC_RETVAL_1394_TIMEOUT		= 6,
	EFC_RETVAL_DSP_TIMEOUT		= 7,
	EFC_RETVAL_BAD_RATE		= 8,
	EFC_RETVAL_BAD_CLOCK		= 9,
	EFC_RETVAL_BAD_CHANNEL		= 10,
	EFC_RETVAL_BAD_PAN		= 11,
	EFC_RETVAL_FLASH_BUSY		= 12,
	EFC_RETVAL_BAD_MIRROR		= 13,
	EFC_RETVAL_BAD_LED		= 14,
	EFC_RETVAL_BAD_PARAMETER	= 15,
	EFC_RETVAL_INCOMPLETE		= 0x80000000
};

static int
efc_over_avc(struct snd_efw_t *efw, unsigned int version,
		unsigned int category, unsigned int command,
		const u32 *params, unsigned int param_count,
		void *response, unsigned int response_quadlets)
{
	int err;

	unsigned int cmdbuf_bytes;
	__be32 *cmdbuf;
	struct efc_fields_t *efc_fields;
	u32 sequence_number;
	unsigned int i;

	/* AV/C fields */
	struct avc_fields_t avc_fields = {
		.cts		= AVC_CTS,
		.ctype		= AVC_CTYPE,
		.subunit_type	= AVC_SUBUNIT_TYPE,
		.subunit_id	= AVC_SUBUNIT_ID,
		.opcode		= AVC_OPCODE,
		.company_id	= AVC_COMPANY_ID
	};

	/* calcurate buffer size*/
	if (param_count > response_quadlets)
		cmdbuf_bytes = 32 + param_count * 4;
	else
		cmdbuf_bytes = 32 + response_quadlets * 4;

	/* keep buffer */
	cmdbuf = kmalloc(cmdbuf_bytes, GFP_KERNEL);
	if (cmdbuf == NULL) {
		err = -ENOMEM;
		goto end;
	}

	/* fill AV/C fields */
	cmdbuf[0] = (avc_fields.cts << 28) | (avc_fields.ctype << 24) |
			(avc_fields.subunit_type << 19) | (avc_fields.subunit_id << 16) |
			(avc_fields.opcode << 8) | (avc_fields.company_id >> 16 & 0xFF);
	cmdbuf[1] = ((avc_fields.company_id >> 8 & 0xFF) << 24) |
			((avc_fields.company_id & 0xFF) << 16) | (0x00 << 8) | 0x00;

	/* fill EFC fields */
	efc_fields = (struct efc_fields_t *)(cmdbuf + 2);
	efc_fields->length	= sizeof(struct efc_fields_t) / 4 + param_count;
	efc_fields->version	= version;
	efc_fields->category	= category;
	efc_fields->command	= command;
	efc_fields->retval	= 0;

	/* sequence number should keep consistency */
	spin_lock(&efw->lock);
	efc_fields->seqnum = efw->sequence_number++;
	sequence_number = efw->sequence_number;
	spin_unlock(&efw->lock);

	/* fill EFC parameters */
	for (i = 0; i < param_count; i += 1)
		cmdbuf[8 + i] = params[i];

	/* for endian-ness */
	for (i = 0; i < (cmdbuf_bytes / 4); i += 1)
		cmdbuf[i] = cpu_to_be32(cmdbuf[i]);

	/* if return value is positive, it means return bytes */
	err = fcp_avc_transaction(efw->unit, cmdbuf, cmdbuf_bytes,
				cmdbuf, cmdbuf_bytes, 0x00);
	if (err < 0)
		goto end;

	/* for endian-ness */
	for (i = 0; i < (err / 4); i += 1)
		cmdbuf[i] = cpu_to_be32(cmdbuf[i]);

	/* parse AV/C fields */
	avc_fields.cts = cmdbuf[0] >> 28;
	avc_fields.ctype = cmdbuf[0] >> 24 & 0x0F;
	avc_fields.subunit_type = cmdbuf[0] >> 19 & 0x1F;
	avc_fields.subunit_id = cmdbuf[0] >> 16 & 0x07;
	avc_fields.opcode = cmdbuf[0] >> 8 & 0xFF;
	avc_fields.company_id = ((cmdbuf[0] & 0xFF) << 16) |
			((cmdbuf[1] >> 24 & 0xFF) << 8) | (cmdbuf[1] >> 16 & 0xFF);

	/* check AV/C fields */
	if ((avc_fields.cts != AVC_CTS) ||
	    (avc_fields.ctype != 0x09) ||	/* just ACCEPTED */
	    (avc_fields.subunit_type != AVC_SUBUNIT_TYPE) ||
	    (avc_fields.subunit_id != AVC_SUBUNIT_ID) ||
	    (avc_fields.opcode != AVC_OPCODE) ||
	    (avc_fields.company_id != AVC_COMPANY_ID)) {
		snd_printk(KERN_INFO "AV/C Failed: 0x%X 0x%X 0x%X 0x%X 0x%X, 0x%X\n",
			avc_fields.cts, avc_fields.ctype, avc_fields.subunit_type,
			avc_fields.subunit_id, avc_fields.opcode, avc_fields.company_id);
		err = -EIO;
		goto end;
	}

	/* check EFC response fields */
	efc_fields = (struct efc_fields_t *)(cmdbuf + 2);
	if ((efc_fields->seqnum != sequence_number) |
	    (efc_fields->category != category) |
	    (efc_fields->command != command) |
	    (efc_fields->retval != EFC_RETVAL_OK)) {
		snd_printk(KERN_INFO "EFC Failed [%u/%u]: %X\n",
			efc_fields->category, efc_fields->command, efc_fields->retval);
		err = -EIO;
		goto end;
	} else {
		err = 0;
	}

	/* fill response buffer */
	memset(response, 0, response_quadlets);
	if (response_quadlets > efc_fields->length)
		response_quadlets = efc_fields->length;
	memcpy(response, cmdbuf + 8, response_quadlets * 4);

end:
	if (cmdbuf != NULL)
		kfree(cmdbuf);
	return err;
}

int
snd_efw_command_identify(struct snd_efw_t *efw)
{
	return 0;
}

int
snd_efw_command_get_hwinfo(void)
{
	return 0;
}

int
snd_efw_command_get_polled(struct snd_efw_t *efw, u32 *value, int count)
{
	return 0;
}

int
snd_efw_command_get_phys_meters_count(struct snd_efw_t *efw, int *inputs, int *outputs)
{	
	return 0;
}

int snd_efw_command_get_phys_meters(struct snd_efw_t *efw,
					int count, u32 *polled_meters)
{
	return 0;
}

static int
snd_efw_command_get_clock(struct snd_efw_t *efw, struct efc_clock_t *clock)
{
	return 0;
}

static int
snd_efw_command_set_clock(struct snd_efw_t *efw,
			int source, int sampling_rate)
{
	return 0;
}

int
snd_efw_command_get_clock_source(struct snd_efw_t *efw,
					enum snd_efw_clock_source_t *source)
{
	return 0;
}

int
snd_efw_command_set_clock_source(struct snd_efw_t *efw,
					enum snd_efw_clock_source_t source)
{
	return snd_efw_command_set_clock(efw, source, -1);
}

int
snd_efw_command_get_sampling_rate(struct snd_efw_t *efw, int *sampling_rate)
{	
	return 0;
}

int
snd_efw_command_set_sampling_rate(struct snd_efw_t *efw, int sampling_rate)
{
	return 0;
}

int snd_efw_command_get_mixer_usable(struct snd_efw_t *efw, int *usable)
{	
	*usable = 0;
	return 0;
}

int snd_efw_command_set_mixer_usable(struct snd_efw_t *efw, int usable)
{
	return 0;
}

int snd_efw_command_get_iec60958_format(struct snd_efw_t *efw,
				enum snd_efw_iec60958_format_t *format)
{
	*format = SND_EFW_IEC60958_FORMAT_PROFESSIONAL;
	return 0;
}

int snd_efw_command_set_iec60958_format(struct snd_efw_t *efw,
				enum snd_efw_iec60958_format_t format)
{
	return 0;
}

int snd_efw_command_get_digital_mode(struct snd_efw_t *efw,
				enum snd_efw_digital_mode_t *mode)
{
	return 0;
}

int snd_efw_command_set_digital_mode(struct snd_efw_t *efw,
				enum snd_efw_digital_mode_t mode)
{
	return 0;
}

int snd_efw_command_get_phantom_state(struct snd_efw_t *efw, int *state)
{
	return 0;
}

int snd_efw_command_set_phantom_state(struct snd_efw_t *efw, int state)
{
	return 0;
}

static int
snd_efw_command_get_monitor(struct snd_efw_t *efw, int category, int command,
						int input, int output, int *value)
{
	return 0;
}

static int
snd_efw_command_set_monitor(struct snd_efw_t *efw, int category, int command,
						int input, int output, int *value)
{
	return 0;
}

int snd_efw_command_monitor(struct snd_efw_t *efw, enum snd_efw_mixer_cmd_t cmd,
						int input, int output, int *value)
{
	switch (cmd) {
	case SND_EFW_MIXER_SET_GAIN:
	case SND_EFW_MIXER_SET_MUTE:
	case SND_EFW_MIXER_SET_SOLO:
	case SND_EFW_MIXER_SET_PAN:
	case SND_EFW_MIXER_SET_NOMINAL:
		return snd_efw_command_set_monitor(efw, EFC_CAT_MIXER_MONITOR, cmd,
								input, output, value);

	case SND_EFW_MIXER_GET_GAIN:
	case SND_EFW_MIXER_GET_MUTE:
	case SND_EFW_MIXER_GET_SOLO:
	case SND_EFW_MIXER_GET_PAN:
		return snd_efw_command_get_monitor(efw, EFC_CAT_MIXER_MONITOR, cmd,
								input, output, value);

	case SND_EFW_MIXER_GET_NOMINAL:
	default:
		return -EINVAL;
	}
}

static int
snd_efw_command_get_mixer(struct snd_efw_t *efw, int category, int command,
						int channel, int *value)
{
	return 0;
}

static int
snd_efw_command_set_mixer(struct snd_efw_t *efw, int category, int command,
						int channel, int *value)
{
	return 0;
}

int snd_efw_command_playback(struct snd_efw_t *efw, enum snd_efw_mixer_cmd_t cmd,
						int channel, int *value)
{
	switch (cmd) {
	case SND_EFW_MIXER_SET_GAIN:
	case SND_EFW_MIXER_SET_MUTE:
	case SND_EFW_MIXER_SET_SOLO:
		return snd_efw_command_set_mixer(efw, EFC_CAT_MIXER_PLAYBACK, cmd,
								channel, value);

	case SND_EFW_MIXER_GET_GAIN:
	case SND_EFW_MIXER_GET_MUTE:
	case SND_EFW_MIXER_GET_SOLO:
		return snd_efw_command_get_mixer(efw, EFC_CAT_MIXER_PLAYBACK, cmd,
								channel, value);

	case SND_EFW_MIXER_SET_PAN:
	case SND_EFW_MIXER_SET_NOMINAL:
	case SND_EFW_MIXER_GET_PAN:
	case SND_EFW_MIXER_GET_NOMINAL:
	default:
		return -EINVAL;
	}
}

int snd_efw_command_phys_out(struct snd_efw_t *efw, enum snd_efw_mixer_cmd_t cmd,
						int channel, int *value)
{
	switch (cmd) {
	case SND_EFW_MIXER_SET_GAIN:
	case SND_EFW_MIXER_SET_MUTE:
	case SND_EFW_MIXER_SET_NOMINAL:
		return snd_efw_command_set_mixer(efw, EFC_CAT_MIXER_PHYS_OUT, cmd,
								channel, value);

	case SND_EFW_MIXER_GET_GAIN:
	case SND_EFW_MIXER_GET_MUTE:
	case SND_EFW_MIXER_GET_NOMINAL:
		return snd_efw_command_get_mixer(efw, EFC_CAT_MIXER_PHYS_OUT, cmd,
								channel, value);

	case SND_EFW_MIXER_SET_SOLO:
	case SND_EFW_MIXER_SET_PAN:
	case SND_EFW_MIXER_GET_SOLO:
	case SND_EFW_MIXER_GET_PAN:
	default:
		return -EINVAL;
	}
}

int snd_efw_command_capture(struct snd_efw_t *efw, enum snd_efw_mixer_cmd_t cmd,
						int channel, int *value)
{
	switch (cmd) {
	case SND_EFW_MIXER_SET_GAIN:
	case SND_EFW_MIXER_SET_MUTE:
	case SND_EFW_MIXER_SET_SOLO:
	case SND_EFW_MIXER_SET_PAN:
	case SND_EFW_MIXER_SET_NOMINAL:
		return snd_efw_command_set_mixer(efw, EFC_CAT_MIXER_CAPTURE, cmd,
								channel, value);

	/* get nothing */
	case SND_EFW_MIXER_GET_GAIN:
	case SND_EFW_MIXER_GET_MUTE:
	case SND_EFW_MIXER_GET_SOLO:
	case SND_EFW_MIXER_GET_PAN:
	case SND_EFW_MIXER_GET_NOMINAL:
	default:
		return -EINVAL;
	}
}

int snd_efw_command_phys_in(struct snd_efw_t *efw, enum snd_efw_mixer_cmd_t cmd,
						int channel, int *value)
{
	switch (cmd) {
	case SND_EFW_MIXER_SET_NOMINAL:
		return snd_efw_command_set_mixer(efw, EFC_CAT_MIXER_PHYS_IN, cmd,
								channel, value);

	/* set nothing except nominal */
	case SND_EFW_MIXER_SET_GAIN:
	case SND_EFW_MIXER_SET_MUTE:
	case SND_EFW_MIXER_SET_SOLO:
	case SND_EFW_MIXER_SET_PAN:
	/* get nothing */
	case SND_EFW_MIXER_GET_GAIN:
	case SND_EFW_MIXER_GET_MUTE:
	case SND_EFW_MIXER_GET_SOLO:
	case SND_EFW_MIXER_GET_NOMINAL:
	case SND_EFW_MIXER_GET_PAN:
	default:
		return -EINVAL;
	}
}
