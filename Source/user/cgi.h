#ifndef CGI_H
#define CGI_H

#include "httpd.h"

int tplCounter(HttpdConnData *connData, char *token, void **arg);

#endif
