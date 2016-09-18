#ifndef ZCD_H
#define ZCD_H

#include <esp8266.h>

#define ZCD_IO_MUX          PERIPHS_IO_MUX_MTMS_U
#define ZCD_IO_NUM          14
#define ZCD_IO_FUNC         FUNC_GPIO14

/* defines the power line INTERVAL (in Hz) */
#define ZCD_FREQUENCY       50
/* defines the maximal regular frequency drift of the power line frequency (in %) */
#define ZCD_FREQUENCY_DRIFT 10

/* defines the interval time of one sine wave (in µs) */
#define ZCD_INTERVAL            ((1000 * 1000) / ZCD_FREQUENCY)
#define ZCD_INTERVAL_DIRFT     (ZCD_INTERVAL * ZCD_FREQUENCY_DRIFT / 100)
#define ZCD_INTERVAL_MIN        (ZCD_INTERVAL - ZCD_INTERVAL_DIRFT)
#define ZCD_INTERVAL_MAX        (ZCD_INTERVAL + ZCD_INTERVAL_DIRFT)

void zcd_init(void);


#endif // ZCD_H
