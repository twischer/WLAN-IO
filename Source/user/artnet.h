#ifndef CONFIG_H_
#define CONFIG_H_

#include <c_types.h>

uint8_t dmx_data[513];
//static uint8_t reply_transmit;

void artnet_init();
void artnet_subNetAddr(const uint8 addr);
void artnet_universeAddr(const uint8 addr);
void artnet_pwmStartAddr(const uint16 addr);

#endif
