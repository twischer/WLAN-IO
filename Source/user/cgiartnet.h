#ifndef CGIARTNET_H
#define CGIARTNET_H

#include "httpd.h"

int cgiArtNet_Template(HttpdConnData *connData, char *token, void **arg);
int cgiArtNetSave(HttpdConnData *connData);

#endif
