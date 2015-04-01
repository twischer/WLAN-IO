/*
Cgi/template routines for the /artnet url.
*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain 
 * this notice you can do whatever you want with this stuff. If we meet some day, 
 * and you think this stuff is worth it, you can buy me a beer in return. 
 * ----------------------------------------------------------------------------
 */


#include <string.h>
#include <osapi.h>
#include "user_interface.h"
#include "artnet.h"
#include "cgi.h"
#include "espmissingincludes.h"
#include "cgiartnet.h"


int ICACHE_FLASH_ATTR cgiArtNetSave(HttpdConnData *connData)
{
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}
	
	char subNetStr[4];
	char universeStr[4];
	char pwmStartStr[4];
	httpdFindArg(connData->post->buff, "subnet", subNetStr, sizeof(subNetStr));
	httpdFindArg(connData->post->buff, "universe", universeStr, sizeof(universeStr));
	httpdFindArg(connData->post->buff, "pwmaddr", pwmStartStr, sizeof(pwmStartStr));

	artnet_subNet = atoi(subNetStr);
	artnet_outputUniverse = atoi(universeStr);
	artnet_pwmStartAddr = atoi(pwmStartStr);
	artnet_saveConfig();

	httpdRedirect(connData, "index.tpl");

	return HTTPD_CGI_DONE;
}
