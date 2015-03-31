/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain 
 * this notice you can do whatever you want with this stuff. If we meet some day, 
 * and you think this stuff is worth it, you can buy me a beer in return. 
 * ----------------------------------------------------------------------------
 */

#include "mem.h"
#include "c_types.h"
#include "user_interface.h"
#include "cgiflash.h"
#include "auth.h"
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

#define BTNGPIO 13

#define at_procTaskPrio        0
#define at_procTaskQueueLen    1

#define at_dmxTaskPrio        1
#define at_dmxTaskQueueLen    1

static ETSTimer resetBtntimer;

os_event_t    at_procTaskQueue[at_procTaskQueueLen];
static void at_procTask(os_event_t *events);
os_event_t    at_dmxTaskQueue[at_dmxTaskQueueLen];
static void at_dmxTask(os_event_t *events);

//Function that tells the authentication system what users/passwords live on the system.
//This is disabled in the default build; if you want to try it, enable the authBasic line in
//the builtInUrls below.
int myPassFn(HttpdConnData *connData, int no, char *user, int userLen, char *pass, int passLen) {
	if (no==0) {
		os_strcpy(user, "admin");
		os_strcpy(pass, "s3cr3t");
		return 1;
//Add more users this way. Check against incrementing no for each user added.
//	} else if (no==1) {
//		os_strcpy(user, "user1");
//		os_strcpy(pass, "something");
//		return 1;
	}
	return 0;
}


/*
This is the main url->function dispatching data struct.
In short, it's a struct with various URLs plus their handlers. The handlers can
be 'standard' CGI functions you wrote, or 'special' CGIs requiring an argument.
They can also be auth-functions. An asterisk will match any url starting with
everything before the asterisks; "*" matches everything. The list will be
handled top-down, so make sure to put more specific rules above the more
general ones. Authorization things (like authBasic) act as a 'barrier' and
should be placed above the URLs they protect.
*/
HttpdBuiltInUrl builtInUrls[]={
	{"/", cgiRedirect, "/index.tpl"},
	{"/flash.bin", cgiReadFlash, NULL},
	{"/led.tpl", cgiEspFsTemplate, tplLed},
	{"/index.tpl", cgiEspFsTemplate, tplCounter},
	{"/led.cgi", cgiLed, NULL},
	{"/updateweb.cgi", cgiUploadEspfs, NULL},

	//Routines to make the /wifi URL and everything beneath it work.

//Enable the line below to protect the WiFi configuration with an username/password combo.
//	{"/wifi/*", authBasic, myPassFn},

	{"/wifi", cgiRedirect, "/wifi/wifi.tpl"},
	{"/wifi/", cgiRedirect, "/wifi/wifi.tpl"},
	{"/wifi/wifiscan.cgi", cgiWiFiScan, NULL},
	{"/wifi/wifi.tpl", cgiEspFsTemplate, tplWlan},
	{"/wifi/connect.cgi", cgiWiFiConnect, NULL},
	{"/wifi/setmode.cgi", cgiWifiSetMode, NULL},

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
		os_printf("KEY pressed!\n");
	} else {
		if (resetCnt >= 30)
		{
			system_restart();
		}
		resetCnt = 0;
	}
}


static void ICACHE_FLASH_ATTR at_dmxTask(os_event_t *event)
{
//	static uint16 cnt = 0;
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
		os_delay_us(54);
	}
	
}

//Main routine. Initialize stdout, the I/O and the webserver and we're done.
void user_init(void) {
	unsigned char a = 0;
	stdoutInit();
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
	
	
	//System LED
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
	gpio_output_set(0, BIT14, BIT14, 0); 
	
	if (!GPIO_INPUT_GET(BTNGPIO))
	{	
		os_printf("Default");
		gpio_output_set(BIT14, 0, BIT14, 0);
		while(!GPIO_INPUT_GET(BTNGPIO));		
		//set default
		os_printf("System set to default!\n");
		char passwd[128] ={'R','A','D','I','G','1','2','3'};
		wifi_set_opmode(3);
		
		struct softap_config apConfig;
		wifi_softap_get_config(&apConfig);
		os_strncpy((char*)apConfig.password, passwd, 64);
		
		apConfig.authmode = 4;
		apConfig.max_connection = 4;
		apConfig.channel = 2;
		
		wifi_softap_set_config(&apConfig);
		wifi_softap_dhcps_start();
		
		wifi_station_dhcpc_stop();
		wifi_station_set_auto_connect(0);
		
		for (a=0;a<10;a++)
		{
			gpio_output_set(BIT14, 0, BIT14, 0);
			os_delay_us(100000);
			gpio_output_set(0, BIT14, BIT14, 0); 
			os_delay_us(100000);
		}
	}
	
	system_os_task(at_procTask, at_procTaskPrio, at_procTaskQueue, at_procTaskQueueLen);
	system_os_task(at_dmxTask, at_dmxTaskPrio, at_dmxTaskQueue, at_dmxTaskQueueLen);
    system_os_post(at_procTaskPrio, 0, 0 );
	
	os_timer_disarm(&resetBtntimer);
	os_timer_setfn(&resetBtntimer, resetBtnTimerCb, NULL);
	os_timer_arm(&resetBtntimer, 40, 1);
	
	httpdInit(builtInUrls, 80);
	artnet_init();
	os_printf("\nReady\n");
}
