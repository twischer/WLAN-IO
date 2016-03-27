#ifndef ZCD_H
#define ZCD_H

#include <esp8266.h>

#define ZCD_IO_MUX PERIPHS_IO_MUX_MTMS_U
#define ZCD_IO_NUM 14
#define ZCD_IO_FUNC  FUNC_GPIO14

void zcd_init(void);


#endif // ZCD_H
