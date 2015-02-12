#include "mem.h"
#include "c_types.h"
#include "user_interface.h"
#include "espmissingincludes.h"
#include "osapi.h"
#include "espconn.h"
#include "ets_sys.h"
#include "httpd.h"
#include "httpdespfs.h"
#include "cgi.h"
#include "cgiwifi.h"
#include "stdout.h"
#include "gpio.h"
#include "artnet.h"
#include "io.h"

#define BTNGPIO 0

#define at_procTaskPrio        0
#define at_procTaskQueueLen    1

#define at_dmxTaskPrio        1
#define at_dmxTaskQueueLen    1

static ETSTimer resetBtntimer;

os_event_t    at_procTaskQueue[at_procTaskQueueLen];
static void at_procTask(os_event_t *events);
os_event_t    at_dmxTaskQueue[at_dmxTaskQueueLen];
static void at_dmxTask(os_event_t *events);

HttpdBuiltInUrl builtInUrls[]={
	{"/", cgiRedirect, "/index.tpl"},
	{"/flash.bin", cgiReadFlash, NULL},
	{"/led.tpl", cgiEspFsTemplate, tplLed},
	{"/dht22.tpl", cgiEspFsTemplate, tplDHT},
	{"/index.tpl", cgiEspFsTemplate, tplCounter},
	{"/led.cgi", cgiLed, NULL},

	//Routines to make the /wifi URL and everything beneath it work.
	{"/wifi", cgiRedirect, "/wifi/wifi.tpl"},
	{"/wifi/", cgiRedirect, "/wifi/wifi.tpl"},
	{"/wifi/wifiscan.cgi", cgiWiFiScan, NULL},
	{"/wifi/wifi.tpl", cgiEspFsTemplate, tplWlan},
	{"/wifi/connect.cgi", cgiWiFiConnect},

	{"*", cgiEspFsHook, NULL}, //Catch-all cgi function for the filesystem
	{NULL, NULL, NULL}
};

static void ICACHE_FLASH_ATTR
at_procTask(os_event_t *events)
{
	system_os_post(at_procTaskPrio, 0, 0 );
}

void at_recvTask() {
}

static void ICACHE_FLASH_ATTR resetBtnTimerCb(void *arg) {
	static int resetCnt=0;
	
	system_os_post(at_dmxTaskPrio, 0, 0 );

	if (!GPIO_INPUT_GET(BTNGPIO)) {
		resetCnt++;
	} else {
		if (resetCnt>=120) { //3 sec pressed
			wifi_station_disconnect();
			wifi_set_opmode(0x3); //reset to AP+STA mode
			os_printf("Reset to AP mode. Restarting system...\n");
			system_restart();
		}
		resetCnt=0;
	}
}


static void ICACHE_FLASH_ATTR at_dmxTask(os_event_t *event)
{
	uint16 i;
  
	//BREAK
	gpio_output_set(0, BIT2, BIT2, 0); 
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	os_delay_us(150);
	
	//MARK
	gpio_output_set(BIT2, 0, BIT2, 0);
	os_delay_us(54);
	
	//START CODE + DMX DATA
	uart_tx_one_char(1, dmx_data[0]);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_U1TXD_BK);
	os_delay_us(54);
	
	//DMX data
	for (i = 1; i < 513; i++)
	{
		uart_tx_one_char(1, dmx_data[i]);
		os_delay_us(50);
	}
	
}

void user_init(void) {

	char passwd[128] ={'R','A','D','I','G','1','2','3'};
	
	stdoutInit();
	ioInit();
	
	wifi_set_opmode(3);
	struct softap_config apConfig;
	wifi_softap_get_config(&apConfig);
	//os_strncpy((char*)apConfig.password, passwd, 64);
	apConfig.authmode = AUTH_OPEN;
	wifi_softap_set_config(&apConfig);
		
	system_os_task(at_procTask, at_procTaskPrio, at_procTaskQueue, at_procTaskQueueLen);
	system_os_task(at_dmxTask, at_dmxTaskPrio, at_dmxTaskQueue, at_dmxTaskQueueLen);
    system_os_post(at_procTaskPrio, 0, 0 );
	
	os_timer_disarm(&resetBtntimer);
	os_timer_setfn(&resetBtntimer, resetBtnTimerCb, NULL);
	os_timer_arm(&resetBtntimer, 30, 1);
  
	httpdInit(builtInUrls, 80);
	artnet_init();
	os_printf("\nReady OK\n");
}
