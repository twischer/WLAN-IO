#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

#include "user_interface.h"
#include "ets_sys.h"
#include "uart_hw.h"

#define SYS_CLK_MHZ		SYS_CPU_80MHZ


/* enables the stdout and prints denug messages */
#define DEBUG

/*
 * Enables the dmx output.
 * Do not activating debug and dmx output,
 * if dmx output is configured to use the same uart port
 * as the debug output.
 */
//#define USE_DMX_OUTPUT

#ifdef USE_DMX_OUTPUT
#define DMX_IO_MUX		PERIPHS_IO_MUX_U0TXD_U
#define DMX_IO_GPIO		FUNC_GPIO1
#define DMX_IO_TXD		FUNC_U0TXD
#define DMX_IO_NUM		1
#define DMX_IO_BIT		BIT1
#define DMX_UART		UART0
#endif


/* MQTT configurations */
#define MQTT_HOST				"192.168.11.122" //or "mqtt.yourdomain.com"
#define MQTT_PORT				1880
#define MQTT_BUF_SIZE			1024
#define MQTT_KEEPALIVE			120	 /*second*/

#define MQTT_CLIENT_ID			"DVES_%08X"
#define MQTT_USER				"DVES_USER"
#define MQTT_PASS				"DVES_PASS"

#define MQTT_RECONNECT_TIMEOUT	5	/*second*/

#define DEFAULT_SECURITY		0
#define QUEUE_BUFFER_SIZE		2048

#define PROTOCOL_NAMEv31		/*MQTT version 3.1 compatible with Mosquitto v0.15*/
//PROTOCOL_NAMEv311				/*MQTT version 3.11 compatible with https://eclipse.org/paho/clients/testing/*/


/* debug configuration */
#ifdef DEBUG
#define PDBG	os_printf
#else
#define PDBG	nullPrint
#endif

inline void nullPrint(const char *format, ...) {}

#endif /* _USER_CONFIG_H_ */
