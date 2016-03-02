#ifndef CGIFLASH_H
#define CGIFLASH_H

#include "httpd.h"

uint32* const getNextFlashAddr(void);
const char* const checkUpgradedFirmware(void);

int cgiReadFlash(HttpdConnData *connData);
int cgiGetFirmwareNext(HttpdConnData *connData);
int cgiUploadFirmware(HttpdConnData *connData);
int cgiRebootFirmware(HttpdConnData *connData);
int cgiReset(HttpdConnData *connData);

#endif
