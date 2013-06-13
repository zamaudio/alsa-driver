/*
 * 003_lowlevel.c  Digidesign 003Rack driver
 *
 * Copyright (c) Clemens Ladisch <clemens@ladisch.de>
 * Copyright (C) 2012 Robin Gareus <robin@gareus.org>
 * Copyright (C) 2012 Damien Zammit <damien@zamaudio.com>
 * Licensed under the terms of the GNU General Public License, version 2.
 */

#include "003.h"

#define R003_HARDWARE_ADDR      0xffff00000000ULL

#define VENDOR_DIGIDESIGN       0x00a07e
#define VENDOR_DIGIDESIGN_NAME  " "
#define R003_MODEL_ID           0x00ab0000
//#define R003_MODEL_ID           0x00000002
#define R003_MODEL_NAME         " 003Rack "

#define R003_STREAMS_W_REG      0xe0000004
#define R003_STREAMS_R_REG      0xe0000000
#define R003_STREAMS_OFF        0x00000000
#define R003_STREAMS_ON         0x00000001
#define R003_STREAMS_INIT       0x00000002
#define R003_STREAMS_SHUTDOWN   0x00000003

#define R003_SAMPLERATE_REG     0xe0000110
#define R003_SAMPLERATE_44100   0x00000000
#define R003_SAMPLERATE_48000   0x00000001
#define R003_SAMPLERATE_88200   0x00000002
#define R003_SAMPLERATE_96000   0x00000003

#define R003_CLOCKSOURCE_REG    0xe0000118
#define R003_CLOCK_INTERNAL     0x00000000
#define R003_CLOCK_SPDIF        0x00000001
#define R003_CLOCK_ADAT         0x00000002
#define R003_CLOCK_WORDCLOCK    0x00000003

#define R003_MIX                (0xe0000300 | R003_HARDWARE_ADDR)
#define R003_MIX_ANALOG_1L      (0x00 | R003_MIX)
#define R003_MIX_ANALOG_1R      (0x04 | R003_MIX)
#define R003_MIX_ANALOG_2L      (0x08 | R003_MIX)
#define R003_MIX_ANALOG_2R      (0x0c | R003_MIX)
#define R003_MIX_ANALOG_3L      (0x10 | R003_MIX)
#define R003_MIX_ANALOG_3R      (0x14 | R003_MIX)
#define R003_MIX_ANALOG_4L      (0x18 | R003_MIX)
#define R003_MIX_ANALOG_4R      (0x1c | R003_MIX)
#define R003_MIX_ANALOG_5L      (0x20 | R003_MIX)
#define R003_MIX_ANALOG_5R      (0x24 | R003_MIX)
#define R003_MIX_ANALOG_6L      (0x28 | R003_MIX)
#define R003_MIX_ANALOG_6R      (0x2c | R003_MIX)
#define R003_MIX_ANALOG_7L      (0x30 | R003_MIX)
#define R003_MIX_ANALOG_7R      (0x34 | R003_MIX)
#define R003_MIX_ANALOG_8L      (0x38 | R003_MIX)
#define R003_MIX_ANALOG_8R      (0x3c | R003_MIX)
#define R003_MIX_SPDIF_1L       (0x40 | R003_MIX)
#define R003_MIX_SPDIF_1R       (0x44 | R003_MIX)
#define R003_MIX_SPDIF_2L       (0x48 | R003_MIX)
#define R003_MIX_SPDIF_2R       (0x4c | R003_MIX)
#define R003_MIX_ADAT_1L        (0x50 | R003_MIX)
#define R003_MIX_ADAT_1R        (0x54 | R003_MIX)
#define R003_MIX_ADAT_2L        (0x58 | R003_MIX)
#define R003_MIX_ADAT_2R        (0x5c | R003_MIX)
#define R003_MIX_ADAT_3L        (0x60 | R003_MIX)
#define R003_MIX_ADAT_3R        (0x64 | R003_MIX)
#define R003_MIX_ADAT_4L        (0x68 | R003_MIX)
#define R003_MIX_ADAT_4R        (0x6c | R003_MIX)
#define R003_MIX_ADAT_5L        (0x70 | R003_MIX)
#define R003_MIX_ADAT_5R        (0x74 | R003_MIX)
#define R003_MIX_ADAT_6L        (0x78 | R003_MIX)
#define R003_MIX_ADAT_6R        (0x7c | R003_MIX)
#define R003_MIX_ADAT_7L        (0x80 | R003_MIX)
#define R003_MIX_ADAT_7R        (0x84 | R003_MIX)
#define R003_MIX_ADAT_8L        (0x88 | R003_MIX)
#define R003_MIX_ADAT_8R        (0x8c | R003_MIX)

#define R003_MIX_NONE           0x00000000
#define R003_MIX_1_TO_STEREO    0x18000000
#define R003_MIX_1_TO_1         0x20000000

#define BYTESWAP32_CONST(x) ((((x) & 0x000000FF) << 24) |   \
                             (((x) & 0x0000FF00) << 8) |    \
                             (((x) & 0x00FF0000) >> 8) |    \
                             (((x) & 0xFF000000) >> 24))

void write_quadlet(struct snd_efw_t *digi, unsigned long long int reg, unsigned int data)
{
	data = BYTESWAP32_CONST(data);
	snd_fw_transaction(digi->unit, TCODE_WRITE_QUADLET_REQUEST, reg, &data, 4);
}

unsigned int read_quadlet(struct snd_efw_t *digi, unsigned long long int reg)
{
	unsigned int data = 0;
	snd_fw_transaction(digi->unit, TCODE_READ_QUADLET_REQUEST, reg, &data, 4);
	return BYTESWAP32_CONST(data);
}

static inline int poll_until(struct snd_efw_t *digi, unsigned long long int reg, unsigned int expect)
{
	int timeout = 1024;
	while (read_quadlet(digi, reg) != expect && --timeout);
	return ( timeout == 0 );
}

static void rack_init_write_814_block(struct snd_efw_t *digi)
{
/*
 * write_block_request, offs=0xffffe0000008, data_length=0x0008, extended_tcode=0x0000, data=[ffc2ffff 00000000]
 * write_block_request, offs=0xffffe0000014, data_length=0x0008, extended_tcode=0x0000, data=[ffc2ffff 00000040]
 */
#if 1   /* use transaction */
	__be32 data[2];
	data[0] = BYTESWAP32_CONST(0xffc2ffff);
	data[1] = BYTESWAP32_CONST(0x00000000);
	snd_fw_transaction(digi->unit, TCODE_WRITE_BLOCK_REQUEST, 0xffffe0000008ULL, &data, 8);

	data[0] = BYTESWAP32_CONST(0xffc2ffff);
	data[1] = BYTESWAP32_CONST(0x00000040);
	snd_fw_transaction(digi->unit, TCODE_WRITE_BLOCK_REQUEST, 0xffffe0000014ULL, &data, 8);

#elif 0 /* write two quadlets instead of continuous block */

	write_quadlet(digi, 0xffffe0000008ULL, 0xffc2ffff);
	write_quadlet(digi, 0xffffe000000cULL, 0x00000000);

	write_quadlet(digi, 0xffffe0000014ULL, 0xffc2ffff);
	write_quadlet(digi, 0xffffe0000018ULL, 0x00000040);
#endif
}

int rack_init(struct snd_efw_t *digi)
{
	/* sweep read all data regs */
	int i;
	for (i=0; i < /* 0x468 */ 0x010; i++) {
		if (i == 4) continue;
		read_quadlet(digi, 0xfffff0000400ULL + i);
	}
	read_quadlet(digi, 0xfffff0000400ULL);

	/* initialization sequence */

	write_quadlet(digi, 0xffffe0000004ULL, 0x00000002);
	if (poll_until(digi, 0xffffe0000000ULL, 0x00000000)) return -1;

	write_quadlet(digi, 0xffffe0000110ULL, 0x00000000); // set samplerate?

	rack_init_write_814_block(digi);

	write_quadlet(digi, 0xffffe0000110ULL, 0x00000001); // set samplerate?
	write_quadlet(digi, 0xffffe0000100ULL, 0x00000000); // ??
	write_quadlet(digi, 0xffffe0000100ULL, 0x00000001); // ??

	write_quadlet(digi, 0xffffe000011cULL, 0x00000000); //use for x44.1?
	write_quadlet(digi, 0xffffe0000120ULL, 0x00000003); //use for x44.1?

	write_quadlet(digi, 0xffffe0000118ULL, 0x00000000); // set clocksrc

	write_quadlet(digi, 0xffffe0000004ULL, 0x00000001); // start streaming
	if (poll_until(digi, 0xffffe0000000ULL, 0x00000001)) return -1;

	read_quadlet(digi, 0xffffe0000118ULL); // reset clock

	write_quadlet(digi, 0xffffe0000004ULL, 0x00000000);
	if (poll_until(digi, 0xffffe0000000ULL, 0x00000000)) return -1;

	write_quadlet(digi, 0xffffe0000124ULL, 0x00000001); //enable midi or low latency?

	write_quadlet(digi, 0xffffe0000004ULL, 0x00000001); // start streaming
	if (poll_until(digi, 0xffffe0000000ULL, 0x00000001)) return -1;

	read_quadlet(digi, 0xffffe0000118ULL); // reset clock

	write_quadlet(digi, 0xffffe0000004ULL, 0x00000000);
	if (poll_until(digi, 0xffffe0000000ULL, 0x00000000)) return -1;

	write_quadlet(digi, 0xffffe0000004ULL, 0x00000003);
	if (poll_until(digi, 0xffffe0000000ULL, 0x00000003)) return -1;

	write_quadlet(digi, 0xffffe0000004ULL, 0x00000002);
	if (poll_until(digi, 0xffffe0000000ULL, 0x00000000)) return -1;

	write_quadlet(digi, 0xffffe0000110ULL, 0x00000000); // set samplerate?

	rack_init_write_814_block(digi);

	write_quadlet(digi, 0xffffe0000110ULL, 0x00000000); // set samplerate?

	write_quadlet(digi, 0xffffe0000100ULL, 0x00000000); // ??
	write_quadlet(digi, 0xffffe0000100ULL, 0x00000001); // ??

	write_quadlet(digi, 0xffffe0000004ULL, 0x00000001); // start streaming
	if (poll_until(digi, 0xffffe0000000ULL, 0x00000001)) return -1;

	read_quadlet(digi, 0xffffe0000118ULL); // reset clock

	write_quadlet(digi, 0xffffe0000124ULL, 0x00000001); // stop control

	write_quadlet(digi, 0xffffe0000004ULL, 0x00000000);
	if (poll_until(digi, 0xffffe0000000ULL, 0x00000000)) return -1;

	write_quadlet(digi, 0xffffe0000004ULL, 0x00000003); // shutdown streaming
	if (poll_until(digi, 0xffffe0000000ULL, 0x00000003)) return -1;

	write_quadlet(digi, 0xffffe0000004ULL, 0x00000002);
	if (poll_until(digi, 0xffffe0000000ULL, 0x00000000)) return -1;

	write_quadlet(digi, 0xffffe0000110ULL, 0x00000000); // set samplerate?

	rack_init_write_814_block(digi);

	write_quadlet(digi, 0xffffe0000110ULL, 0x00000001); // set samplerate?

	write_quadlet(digi, 0xffffe0000100ULL, 0x00000000); // ??
	write_quadlet(digi, 0xffffe0000100ULL, 0x00000001); // ??

	write_quadlet(digi, 0xffffe0000004ULL, 0x00000001); // start streaming
	if (poll_until(digi, 0xffffe0000000ULL, 0x00000001)) return -1;

	read_quadlet(digi, 0xffffe0000118ULL); // reset clock

	write_quadlet(digi, 0xffffe0000124ULL, 0x00000001); // stop control

#if 0
	//write_quadlet(digi, 0xffffe0000124ULL, 0x00000000); // start control
	/* No monitoring of inputs */

	write_quadlet(digi, R003_MIX_ANALOG_1L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ANALOG_1R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ANALOG_2L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ANALOG_2R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ANALOG_3L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ANALOG_3R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ANALOG_4L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ANALOG_4R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ANALOG_5L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ANALOG_5R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ANALOG_6L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ANALOG_6R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ANALOG_7L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ANALOG_7R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ANALOG_8L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ANALOG_8R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_SPDIF_1L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_SPDIF_1R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_SPDIF_1L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_SPDIF_1R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_1L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_1R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_2L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_2R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_3L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_3R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_4L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_4R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_5L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_5R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_6L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_6R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_7L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_7R, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_8L, R003_MIX_NONE);
	write_quadlet(digi, R003_MIX_ADAT_8R, R003_MIX_NONE);
#endif
	return 0;
}

void rack_shutdown(struct snd_efw_t *digi)
{
#if 0
	write_quadlet(digi, 0xffffe0000124ULL, 0x00000001);   // stop control
#endif
	write_quadlet(digi, 0xffffe0000004ULL, 0x00000000);   // stop streams
	poll_until(digi, 0xffffe0000000ULL, 0x00000000);

	write_quadlet(digi, 0xffffe0000004ULL, 0x00000001);   // start streams
	poll_until(digi, 0xffffe0000000ULL, 0x00000001);
	write_quadlet(digi, 0xffffe0000118ULL, 0x00000000);

	write_quadlet(digi, 0xffffe0000004ULL, 0x00000000);   // stop streams
	poll_until(digi, 0xffffe0000000ULL, 0x00000000);

	write_quadlet(digi, 0xffffe0000004ULL, 0x00000003);   // shutdown streams
	poll_until(digi, 0xffffe0000000ULL, 0x00000003);
}

void digi_free_resources(struct snd_efw_t *digi, struct snd_efw_stream_t *stream)
{
	rack_shutdown(digi);
	fw_iso_resources_free(&stream->conn);
}

int digi_allocate_resources(struct snd_efw_t *digi, enum amdtp_stream_direction direction)
{
	int err;

	if (direction == AMDTP_STREAM_RECEIVE) {
		if (digi->receive_stream.conn.allocated)
			return 0;

		err = fw_iso_resources_allocate(&(digi->receive_stream.conn),
			amdtp_out_stream_get_max_payload(&(digi->receive_stream.strm)),
			fw_parent_device(digi->unit)->max_speed);
	} else {
		if (digi->transmit_stream.conn.allocated)
                        return 0;

                err = fw_iso_resources_allocate(&(digi->transmit_stream.conn),
                        amdtp_out_stream_get_max_payload(&(digi->transmit_stream.strm)),                       
                        fw_parent_device(digi->unit)->max_speed);
	}

	if (err < 0)
		return err;
	
	printk("ALLOCATE ISO SUCCEEDED\n");
	return 0;
}

