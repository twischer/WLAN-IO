#ifndef _PARAMCONF_H_
#define _PARAMCONF_H_

#include "c_types.h"

typedef struct {
	uint8 magic;
	uint8 subNet;
	uint8 universe;
	uint16 pwmStartAddr;
} parameter_t;

bool paramconf_load(parameter_t* const param);
void paramconf_save(parameter_t* const param);

#endif
