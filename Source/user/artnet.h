#ifndef CONFIG_H_
#define CONFIG_H_

#include <c_types.h>

uint8_t dmx_data[513];
//static uint8_t reply_transmit;

uint8 artnet_net;
uint8 artnet_subNet;
uint8 artnet_outputUniverse;
uint16 artnet_pwmStartAddr;

void artnet_init();
void artnet_saveConfig();


#endif
