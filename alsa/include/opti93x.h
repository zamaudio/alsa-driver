
#ifndef __OPTi93X_H
#define __OPTi93X_H

/*
    opti93x.h - definitions for OPTi 82c93x chips.
    Copyright (C) 1999 by Massimo Piccioni <dafastidio@libero.it>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "pcm.h"
#include "mixer.h"
#define OPTi93X
#include "opti9xx.h"

#define OPTi93X_INDEX			0x00
#define OPTi93X_DATA			0x01
#define OPTi93X_STATUS			0x02
#define OPTi93X_DDATA			0x03
#define OPTi93X_PORT(codec, r)		((codec)->port + OPTi93X_##r)

#define OPTi93X_MIXOUT_LEFT		0x00
#define OPTi93X_MIXOUT_RIGHT		0x01
#define OPTi93X_CD_LEFT_INPUT		0x02
#define OPTi93X_CD_RIGHT_INPUT		0x03
#define OPTi930_AUX_LEFT_INPUT		0x04
#define OPTi930_AUX_RIGHT_INPUT		0x05
#define OPTi931_FM_LEFT_INPUT		0x04
#define OPTi931_FM_RIGHT_INPUT		0x05
#define OPTi93X_DAC_LEFT		0x06
#define OPTi93X_DAC_RIGHT		0x07
#define OPTi93X_PLAY_FORMAT		0x08
#define OPTi93X_IFACE_CONF		0x09
#define OPTi93X_PIN_CTRL		0x0a
#define OPTi93X_ERR_INIT		0x0b
#define OPTi93X_ID			0x0c
#define OPTi93X_PLAY_UPR_CNT		0x0e
#define OPTi93X_PLAY_LWR_CNT		0x0f
#define OPTi931_AUX_LEFT_INPUT		0x10
#define OPTi931_AUX_RIGHT_INPUT		0x11
#define OPTi93X_LINE_LEFT_INPUT		0x12
#define OPTi93X_LINE_RIGHT_INPUT	0x13
#define OPTi93X_MIC_LEFT_INPUT		0x14
#define OPTi93X_MIC_RIGHT_INPUT		0x15
#define OPTi93X_OUT_LEFT		0x16
#define OPTi93X_OUT_RIGHT		0x17
#define OPTi93X_CAPT_FORMAT		0x1c
#define OPTi93X_CAPT_UPR_CNT		0x1e
#define OPTi93X_CAPT_LWR_CNT		0x1f

#define OPTi93X_TRD			0x20
#define OPTi93X_MCE			0x40
#define OPTi93X_INIT			0x80

#define OPTi93X_MIXOUT_MIC_GAIN		0x20
#define OPTi93X_MIXOUT_LINE		0x00
#define OPTi93X_MIXOUT_CD		0x40
#define OPTi93X_MIXOUT_MIC		0x80
#define OPTi93X_MIXOUT_MIXER		0xc0

#define OPTi93X_STEREO			0x10
#define OPTi93X_LINEAR_8		0x00
#define OPTi93X_ULAW_8			0x20
#define OPTi93X_LINEAR_16_LIT		0x40
#define OPTi93X_ALAW_8			0x60
#define OPTi93X_ADPCM_16		0xa0
#define OPTi93X_LINEAR_16_BIG		0xc0

#define OPTi93X_CAPTURE_PIO		0x80
#define OPTi93X_PLAYBACK_PIO		0x40
#define OPTi93X_AUTOCALIB		0x08
#define OPTi93X_SINGLE_DMA		0x04
#define OPTi93X_CAPTURE_ENABLE		0x02
#define OPTi93X_PLAYBACK_ENABLE		0x01

#define OPTi93X_IRQ_ENABLE		0x02

#define OPTi93X_DMA_REQUEST		0x10
#define OPTi93X_CALIB_IN_PROGRESS	0x20

#define OPTi93X_IRQ_PLAYBACK		0x04
#define OPTi93X_IRQ_CAPTURE		0x08


typedef struct snd_stru_opti93x opti93x_t;

struct snd_stru_opti93x {
	unsigned short port;
	unsigned short irq;
	unsigned short dma1;
	unsigned short dma2;
	snd_dma_t *dma1ptr;
	snd_dma_t *dma2ptr;

	opti9xx_t *chip;
	unsigned short hardware;
	unsigned char image[32];

	unsigned char mce_bit;
	unsigned short mode;
	int mute;

	spinlock_t lock;

	snd_card_t *card;
	snd_pcm_t *pcm;
	snd_pcm_subchn_t *playback_subchn;
	snd_pcm_subchn_t *capture_subchn;
	unsigned int p_dma_size;
	unsigned int c_dma_size;

	snd_kmixer_t *mixer;

	snd_kmixer_element_t *me_mux;
	snd_kmixer_element_t *me_accu;
	snd_kmixer_element_t *me_dig_accu;

	snd_kmixer_element_t *me_mux_cd;
	snd_kmixer_element_t *me_mux_line;
	snd_kmixer_element_t *me_mux_mic;
	snd_kmixer_element_t *me_mux_mix;

	snd_kmixer_element_t *me_in_aux;
	snd_kmixer_element_t *me_in_cd;
	snd_kmixer_element_t *me_in_fm;
	snd_kmixer_element_t *me_in_line;
	snd_kmixer_element_t *me_in_mic;

	snd_kmixer_element_t *me_out_master;

	snd_kmixer_element_t *me_sw_aux;
	snd_kmixer_element_t *me_sw_cd;
	snd_kmixer_element_t *me_sw_dac;
	snd_kmixer_element_t *me_sw_fm;
	snd_kmixer_element_t *me_sw_line;
	snd_kmixer_element_t *me_sw_mgain;
	snd_kmixer_element_t *me_sw_mic;
	snd_kmixer_element_t *me_sw_out;

	snd_kmixer_element_t *me_vol_aux;
	snd_kmixer_element_t *me_vol_cd;
	snd_kmixer_element_t *me_vol_dac;
	snd_kmixer_element_t *me_vol_fm;
	snd_kmixer_element_t *me_vol_igain;
	snd_kmixer_element_t *me_vol_line;		
	snd_kmixer_element_t *me_vol_mic;
	snd_kmixer_element_t *me_vol_out;

	snd_kmixer_element_t *me_adc;
	snd_kmixer_element_t *me_dac;
	snd_kmixer_element_t *me_capture;
	snd_kmixer_element_t *me_playback;

	unsigned int (*set_rate) (opti93x_t *codec, unsigned int rate);
	void (*set_playback_format) (opti93x_t *codec, unsigned char format);
	void (*set_capture_format) (opti93x_t *codec, unsigned char format);
};

#define OPTi93X_MODE_NONE	0x00
#define OPTi93X_MODE_PLAY	0x01
#define OPTi93X_MODE_CAPTURE	0x02
#define OPTi93X_MODE_OPEN	(OPTi93X_MODE_PLAY | OPTi93X_MODE_CAPTURE)


extern void snd_opti93x_interrupt(snd_pcm_t *pcm, unsigned char status);

extern int snd_opti93x_new_pcm(snd_card_t *card, int device, opti9xx_t *chip,
		snd_dma_t *dma1ptr, snd_dma_t *dma2ptr, snd_pcm_t **rpcm);

extern int snd_opti93x_new_mixer(snd_pcm_t *pcm, int device,
							snd_kmixer_t **rmixer);

#endif	/* __OPTi93X_H */

