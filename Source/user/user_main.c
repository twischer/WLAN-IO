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

#define at_dmxTaskPrio        1
#define at_dmxTaskQueueLen    1

//static ETSTimer resetBtntimer;


#ifdef USE_DMX_OUTPUT
os_event_t    at_dmxTaskQueue[at_dmxTaskQueueLen];
static void at_dmxTask(os_event_t *events);
#endif

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
	{"/index.tpl", cgiEspFsTemplate, tplCounter},
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


// TODO auto change back to AP+STA, if no AP
//static void ICACHE_FLASH_ATTR resetBtnTimerCb(void *arg) {
//	static int resetCnt=0;
//
//	system_os_post(at_dmxTaskPrio, 0, 0 );
//
//	if (!GPIO_INPUT_GET(BTNGPIO)) {
//		resetCnt++;
//		PDBG("KEY pressed!\n");
//	} else {
//		if (resetCnt >= 30)
//		{
//			system_restart();
//		}
//		resetCnt = 0;
//	}
//}


#ifdef USE_DMX_OUTPUT
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
#endif

//Main routine. Initialize stdout, the I/O and the webserver and we're done.
void user_init(void) {

//	char passwd[128] ={'R','A','D','I','G','1','2','3'};

	stdoutInit();

	
	wifi_set_opmode(3);
	struct softap_config apConfig;
	wifi_softap_get_config(&apConfig);
	//os_strncpy((char*)apConfig.password, passwd, 64);
	apConfig.authmode = AUTH_OPEN;
	apConfig.max_connection = 4;
	apConfig.channel = 2;
	wifi_softap_set_config(&apConfig);

#ifdef USE_DMX_OUTPUT
	system_os_task(at_dmxTask, at_dmxTaskPrio, at_dmxTaskQueue, at_dmxTaskQueueLen);
#endif

//	os_timer_disarm(&resetBtntimer);
//	os_timer_setfn(&resetBtntimer, resetBtnTimerCb, NULL);
//	os_timer_arm(&resetBtntimer, 30, 1);

	httpdInit(builtInUrls, 80);
	artnet_init();
	PDBG("\nReady\n");
}
