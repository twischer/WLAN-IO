#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_
#include <c_types.h>
#ifdef __WIN32__
#include <_mingw.h>
#endif


/* enter the deep sleep mode,
 * if all PWM channels have to be set to off
 * from MQTT.
 * Note: Use a static IP address to make the reboot process much more faster
 */
#undef SLEEP_IF_ALL_PWMS_OFF
/* deep sleep timeout (in sec) */
#define SLEEP_TIME      15


#ifndef PWM_CHANNEL
/* Defaults to 3 channels,
 * becasue the main use case is a RGB para
 */
#define PWM_CHANNEL 3
#endif

/* define it to invert the PWM output.
 * If defined the PWM wave is first low and secondly high.
 *
 * To mind changing source code for example call make with
 * $ make COMPONENTS="io/mqtt io/pwm" DEFINES="-DPWM_INVERTED"
 * instead.
 */
//#define PWM_INVERTED


#undef SHOW_HEAP_USE
#undef DEBUGIP
#undef SDK_DBG


/* enables the stdout and prints denug messages */
//#define DEBUG

#undef MAIN_NOTICE
#undef CMD_DBG
#undef ESPFS_DBG
#undef CGI_DBG
#undef CGIFLASH_DBG
#undef CGIMQTT_DBG
#undef CGIPINS_DBG
#undef CGIWIFI_DBG
#undef CONFIG_DBG
#undef LOG_DBG
#undef STATUS_DBG
#undef HTTPD_DBG
#undef MQTT_DBG
#undef MQTTCMD_DBG
#undef PKTBUF_DBG
#undef REST_DBG
#undef RESTCMD_DBG
#undef SERBR_DBG
#undef SERLED_DBG
#undef SLIP_DBG
#undef UART_DBG
#undef MDNS_DBG
#undef OPTIBOOT_DBG
#undef SYSLOG_DBG
#undef CGISERVICES_DBG
#define ZCD_DBG

// If defined, the default hostname for DHCP will include the chip ID to make it unique
#undef CHIP_IN_HOSTNAME

#ifndef SYSLOG
#define LOG_DEBUG(format, ...) do { } while(0)
#define LOG_NOTICE(format, ...) do { } while(0)
#define LOG_WARN(format, ...) do { } while(0)
#define LOG_INFO(format, ...) do { } while(0)
#define LOG_ERR(format, ...) do { } while(0)
#endif

#endif
