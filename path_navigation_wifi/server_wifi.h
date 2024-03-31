#ifndef __SERVER_WIFI_H__
#define __SERVER_WIFI_H__

#include "configuration.h"

/*
 * make cleanup of the wifi receive buffer
 */
extern void makeCleanup();

/*
 * send the data using wifi
 */
extern void sendData(char *buffer);


/*
 * initialize the wifi
 */
extern void initWifi();
#endif
