/*
 * fireworks_stream.c - driver for Firewire devices from Echo Digital Audio
 *
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

int
snd_efw_stream_init(struct snd_efw_t *efw, struct snd_efw_stream_t *stream)
{
	enum cmp_direction c_direction;
	enum amdtp_stream_direction s_direction;
	int err;
	unsigned long long mask;

	if (stream == &efw->receive_stream) {
		c_direction = CMP_OUTPUT;
		s_direction = AMDTP_STREAM_RECEIVE;
		mask = 0x0000000000000002uLL;
	} else {
		c_direction = CMP_INPUT;
		s_direction = AMDTP_STREAM_TRANSMIT;
		mask = 0x0000000000000001uLL;
	}

        efw->receive_stream.strm.midi_quadlets[0] = 0;
        efw->receive_stream.strm.pcm_quadlets[0] = 1;
        efw->receive_stream.strm.pcm_quadlets[1] = 2;
        efw->receive_stream.strm.pcm_quadlets[2] = 3;
        efw->receive_stream.strm.pcm_quadlets[3] = 4;
        efw->receive_stream.strm.pcm_quadlets[4] = 5;
        efw->receive_stream.strm.pcm_quadlets[5] = 6;
        efw->receive_stream.strm.pcm_quadlets[6] = 7;
        efw->receive_stream.strm.pcm_quadlets[7] = 8;
        efw->receive_stream.strm.pcm_quadlets[8] = 9;
        efw->receive_stream.strm.pcm_quadlets[9] = 10;
        efw->receive_stream.strm.pcm_quadlets[10] = 11;
        efw->receive_stream.strm.pcm_quadlets[11] = 12;
        efw->receive_stream.strm.pcm_quadlets[12] = 13;
        efw->receive_stream.strm.pcm_quadlets[13] = 14;
        efw->receive_stream.strm.pcm_quadlets[14] = 15;
        efw->receive_stream.strm.pcm_quadlets[15] = 16;
        efw->receive_stream.strm.pcm_quadlets[16] = 17;
        efw->receive_stream.strm.pcm_quadlets[17] = 18;
	
        efw->transmit_stream.strm.midi_quadlets[0] = 0;
        efw->transmit_stream.strm.pcm_quadlets[0] = 1;
        efw->transmit_stream.strm.pcm_quadlets[1] = 2;
        efw->transmit_stream.strm.pcm_quadlets[2] = 3;
        efw->transmit_stream.strm.pcm_quadlets[3] = 4;
        efw->transmit_stream.strm.pcm_quadlets[4] = 5;
        efw->transmit_stream.strm.pcm_quadlets[5] = 6;
        efw->transmit_stream.strm.pcm_quadlets[6] = 7;
        efw->transmit_stream.strm.pcm_quadlets[7] = 8;
        efw->transmit_stream.strm.pcm_quadlets[8] = 9;
        efw->transmit_stream.strm.pcm_quadlets[9] = 10;
        efw->transmit_stream.strm.pcm_quadlets[10] = 11;
        efw->transmit_stream.strm.pcm_quadlets[11] = 12;
        efw->transmit_stream.strm.pcm_quadlets[12] = 13;
        efw->transmit_stream.strm.pcm_quadlets[13] = 14;
        efw->transmit_stream.strm.pcm_quadlets[14] = 15;
        efw->transmit_stream.strm.pcm_quadlets[15] = 16;
        efw->transmit_stream.strm.pcm_quadlets[16] = 17;
        efw->transmit_stream.strm.pcm_quadlets[17] = 18;
	
        err = fw_iso_resources_init(&stream->conn, efw->unit);
        if (err < 0)
		goto err_resources;
        err = fw_iso_resources_init(&stream->conn, efw->unit);
        if (err < 0)
		goto err_resources;

	stream->conn.channels_mask = mask;

	err = digi_allocate_resources(efw, s_direction);
	if (err < 0)
		goto end;


	err = amdtp_out_stream_init(&stream->strm, efw->unit, s_direction, CIP_NONBLOCKING);
	if (err < 0) {
		rack_shutdown(efw);
		digi_free_resources(efw, stream);
		goto end;
	}

	stream->pcm = false;
	stream->midi = false;
	
	return 0;

err_resources:
end:
	printk("wtf iso?\n");
	return err;
}

int
snd_efw_stream_start(struct snd_efw_t *efw, struct snd_efw_stream_t *stream)
{
	int err;

	/* already running */
	if (!IS_ERR(stream->strm.context)) {
		err = 0;
		goto end;
	}

//	rack_init(efw);

	/* start amdtp stream */
	err = amdtp_out_stream_start(&stream->strm,
		stream->conn.channel,
		fw_parent_device(efw->unit)->max_speed);
	if (err < 0)
		snd_efw_stream_stop(efw, stream);

end:
	printk("ERR WTF stream start?\n");
	return err;
}

void
snd_efw_stream_stop(struct snd_efw_t *efw, struct snd_efw_stream_t *stream)
{
	if (!!IS_ERR(stream->strm.context))
		goto end;

	amdtp_out_stream_stop(&stream->strm);
end:
	return;
}

void
snd_efw_stream_destroy(struct snd_efw_t *efw, struct snd_efw_stream_t *stream)
{
	snd_efw_stream_stop(efw, stream);
 	digi_free_resources(efw, stream);
	return;
}
