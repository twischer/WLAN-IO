#ifndef SPARMATICCTRL_H
#define SPARMATICCTRL_H

#include <stdint.h>

void heater_init(void);
void heater_setTemp(const uint8_t temperature);
//void heater_incTemp(void);
//void heater_decTemp(void);
void heater_off(void);
//void heater_restore(void);

#endif // SPARMATICCTRL_H
