/*
 *   Copyright 2015, Timo Wischer <twischer@freenet.de>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CONFIG_H_
#define CONFIG_H_

#include <c_types.h>
#include "user_config.h"

#define MAX_CHANNELS 			512

#ifdef USE_DMX_OUTPUT
uint8_t dmx_data[MAX_CHANNELS];
#endif

uint8 artnet_net;
uint8 artnet_subNet;
uint8 artnet_outputUniverse;
uint16 artnet_pwmStartAddr;

void artnet_init();
void artnet_saveConfig();


#endif
