#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_
#include <c_types.h>
#ifdef __WIN32__
#include <_mingw.h>
#endif

#undef WEBLOGGING
#undef SHOW_HEAP_USE
//#define DEBUGIP
//#define SDK_DBG

#undef ESPFS_DBG
#undef CGI_DBG
//#define CGIFLASH_DBG
//#define CGIMQTT_DBG
//#define CGIPINS_DBG
//#define CGIWIFI_DBG
//#define CONFIG_DBG
//#define LOG_DBG
//#define STATUS_DBG
//#define HTTPD_DBG
//#define MQTT_DBG
//#define MQTTCMD_DBG
#undef PKTBUF_DBG
//#define REST_DBG
//#define RESTCMD_DBG
//#define SERBR_DBG
//#define SERLED_DBG
#undef SLIP_DBG
//#define UART_DBG

// If defined, the default hostname for DHCP will include the chip ID to make it unique
#undef CHIP_IN_HOSTNAME

#define SYS_CLK_MHZ		SYS_CPU_80MHZ


/* enables the stdout and prints denug messages */
//#define DEBUG

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


#ifdef DEBUG
#define PDBG	os_printf
#else
#define PDBG(...)
#endif


extern char* esp_link_version;
extern uint8_t UTILS_StrToIP(const char* str, void *ip);

#endif
