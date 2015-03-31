#ifndef CONFIG_H_
#define CONFIG_H_

uint8_t dmx_data[513];
//static uint8_t reply_transmit;

void artnet_init();
void ICACHE_FLASH_ATTR artnet_sendPollReply(void);


#endif
