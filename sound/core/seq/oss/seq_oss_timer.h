/*
 * OSS compatible sequencer driver
 * timer handling routines
 *
 * Copyright (C) 1998,99 Takashi Iwai <tiwai@suse.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __SEQ_OSS_TIMER_H
#define __SEQ_OSS_TIMER_H

#include "seq_oss_device.h"

/*
 * timer information definition
 */
struct seq_oss_timer_t {
	seq_oss_devinfo_t *dp;
	reltime_t cur_tick;
	int realtime;
	int running;
	int tempo, ppq;	/* ALSA queue */
	int oss_tempo, oss_timebase;
};	


seq_oss_timer_t *snd_seq_oss_timer_new(seq_oss_devinfo_t *dp);
void snd_seq_oss_timer_delete(seq_oss_timer_t *dp);

int snd_seq_oss_timer_start(seq_oss_timer_t *timer);
int snd_seq_oss_timer_stop(seq_oss_timer_t *timer);
int snd_seq_oss_timer_continue(seq_oss_timer_t *timer);
int snd_seq_oss_timer_tempo(seq_oss_timer_t *timer, int value);
#define snd_seq_oss_timer_reset  snd_seq_oss_timer_start

int snd_seq_oss_timer_ioctl(seq_oss_timer_t *timer, unsigned int cmd, void *arg);

/*
 * get current processed time
 */
static inline abstime_t
snd_seq_oss_timer_cur_tick(seq_oss_timer_t *timer)
{
	return timer->cur_tick;
}


/*
 * is realtime event?
 */
static inline int
snd_seq_oss_timer_is_realtime(seq_oss_timer_t *timer)
{
	return timer->realtime;
}

#endif
