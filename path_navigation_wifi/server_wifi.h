#ifndef __SERVER_WIFI_H__
#define __SERVER_WIFI_H__

#include "configuration.h"

/*
 * send the data using wifi
 */
extern void sendData(char *buffer);


/*
 * initialize the wifi
 */
extern void initWifi();
#endif
