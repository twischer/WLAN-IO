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
