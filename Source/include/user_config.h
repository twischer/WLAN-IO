#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_
#include <c_types.h>
#ifdef __WIN32__
#include <_mingw.h>
#endif

#define LOGL_OFF     0
#define LOGL_ERR     1
#define LOGL_WRN     2
#define LOGL_INF     3
#define LOGL_DBG     4


/* enter the deep sleep mode,
 * if all PWM channels have to be set to off
 * from MQTT.
 * Note: Use a static IP address to make the reboot process much more faster
 */
#undef SLEEP_IF_ALL_PWMS_OFF
/* deep sleep timeout (in sec) */
#define SLEEP_TIME      15


#define PWM_CHANNEL 3



#undef WEBLOGGING
#undef SHOW_HEAP_USE
//#define DEBUGIP
//#define SDK_DBG


/* enables the stdout and prints denug messages */
//#define DEBUG

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
#undef MQTTCLIENT_DBG
#undef PKTBUF_DBG
//#define REST_DBG
//#define RESTCMD_DBG
//#define SERBR_DBG
//#define SERLED_DBG
#undef SLIP_DBG
//#define UART_DBG
#define ARTNET_LOGL     LOGL_OFF
#define PWMOUT_LOGL     LOGL_OFF
#define SLEEP_LOGL      LOGL_OFF
#define HEATER_LOGL     LOGL_DBG
#define DHTXX_LOGL      LOGL_DBG


// If defined, the default hostname for DHCP will include the chip ID to make it unique
#undef CHIP_IN_HOSTNAME

#define SYS_CLK_MHZ		SYS_CPU_80MHZ




#ifdef DEBUG
#define LOG(type, logl, fmt, ...) \
    if (logl >= type) { \
        os_printf(#type "[%s:%u]" fmt "\n", __func__, __LINE__, ##__VA_ARGS__); \
    }

#define PERR(...)	LOG(LOGL_ERR, ##__VA_ARGS__)
#define PWRN(...)	LOG(LOGL_WRN, ##__VA_ARGS__)
#define PINF(...)	LOG(LOGL_INF, ##__VA_ARGS__)
#define PDBG(...)	LOG(LOGL_DBG, ##__VA_ARGS__)

#else
#define PERR(...)
#define PWRN(...)
#define PINF(...)
#define PDBG(...)
#endif


extern char* esp_link_version;
extern uint8_t UTILS_StrToIP(const char* str, void *ip);

#endif
