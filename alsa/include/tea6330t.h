#ifndef __TEA6330T_H
#define __TEA6330T_H

/*
 *  Routines for control of TEA6330T circuit.
 *  Sound fader control circuit for car radios.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 */

#include "i2c.h"		/* generic i2c support */
#include "mixer.h"

struct snd_tea6330t_mapping {
	unsigned int priority;
	unsigned int parent_priority;
	char *name;
	unsigned int ossdev;
};

extern int snd_tea6330t_detect(struct snd_i2c_bus *bus, int equalizer);
extern int snd_tea6330t_update_mixer(snd_kmixer_t * mixer,
				     struct snd_i2c_bus *bus,
				     struct snd_tea6330t_mapping *mapping,
				     int equalizer, int fader);

#endif				/* __TEA6330T_H */
