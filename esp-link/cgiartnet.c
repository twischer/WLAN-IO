//#ifdef ARTNET
#include <esp8266.h>
#include "cgi.h"
#include "config.h"
#include "cgiartnet.h"


// Cgi to return MQTT settings
int ICACHE_FLASH_ATTR cgiArtNetGet(HttpdConnData *connData) {
//  char buff[1024];
//  int len;

//  if (connData->conn==NULL) return HTTPD_CGI_DONE;

//  // get the current status topic for display
//  char status_buf1[128], *sb1=status_buf1;
//  char status_buf2[128], *sb2=status_buf2;

//  // quote all " for the json, sigh...
//  for (int i=0; i<127 && *sb1; i++) {
//    if (*sb1 == '"') {
//      *sb2++ = '\\';
//      i++;
//    }
//    *sb2++ = *sb1++;
//  }
//  *sb2 = 0;

//  len = os_sprintf(buff, "{ "
//      "\"slip-enable\":%d, "
//      "\"mqtt-enable\":%d, "
//      "\"mqtt-state\":\"%s\", "
//      "\"mqtt-status-enable\":%d, "
//      "\"mqtt-clean-session\":%d, "
//      "\"mqtt-port\":%d, "
//      "\"mqtt-timeout\":%d, "
//      "\"mqtt-keepalive\":%d, "
//      "\"mqtt-host\":\"%s\", "
//      "\"mqtt-client-id\":\"%s\", "
//      "\"mqtt-username\":\"%s\", "
//      "\"mqtt-password\":\"%s\", "
//      "\"mqtt-status-topic\":\"%s\", "
//      "\"mqtt-status-value\":\"%s\" }",
//      flashConfig.slip_enable, flashConfig.mqtt_enable,
//      mqtt_states[mqttClient.connState], flashConfig.mqtt_status_enable,
//      flashConfig.mqtt_clean_session, flashConfig.mqtt_port,
//      flashConfig.mqtt_timeout, flashConfig.mqtt_keepalive,
//      flashConfig.mqtt_host, flashConfig.mqtt_clientid,
//      flashConfig.mqtt_username, flashConfig.mqtt_password,
//      flashConfig.mqtt_status_topic, status_buf2);

//  jsonHeader(connData, 200);
//  httpdSend(connData, buff, len);
  return HTTPD_CGI_DONE;
}

// Cgi to change choice of pin assignments
int ICACHE_FLASH_ATTR cgiArtNetSet(HttpdConnData *connData) {
  if (connData->conn==NULL) return HTTPD_CGI_DONE;

  // handle Art-Net settings
  char buffer[4];
  if (httpdFindArg(connData->post->buff, "artnet-subnet", buffer, sizeof(buffer)) < 0) {
	return HTTPD_CGI_DONE;
  }
  flashConfig.artnet_subnet = atoi(buffer);
  // TODO
//  errorResponse(connData, 400, "Invalid MQTT port");
//  return HTTPD_CGI_DONE;

  if (httpdFindArg(connData->post->buff, "artnet-universe", buffer, sizeof(buffer)) < 0) {
	  return HTTPD_CGI_DONE;
  }
  flashConfig.artnet_universe = atoi(buffer);


  if (httpdFindArg(connData->post->buff, "artnet-pwmstart", buffer, sizeof(buffer)) < 0) {
	  return HTTPD_CGI_DONE;
  }
  flashConfig.artnet_pwmstart = atoi(buffer);


  PINF(ARTNET_LOGL, "Saving config (sub %u univ %u pwm %u)\n", flashConfig.artnet_subnet, flashConfig.artnet_universe, flashConfig.artnet_pwmstart);

  if (configSave()) {
	httpdRedirect(connData, "/artnet.html");
  } else {
    httpdStartResponse(connData, 500);
    httpdEndHeaders(connData);
    httpdSend(connData, "Failed to save config", -1);
  }
  return HTTPD_CGI_DONE;
}


int ICACHE_FLASH_ATTR cgiArtNet(HttpdConnData *connData) {
  if (connData->requestType == HTTPD_METHOD_GET) {
	return cgiArtNetGet(connData);
  } else if (connData->requestType == HTTPD_METHOD_POST) {
	return cgiArtNetSet(connData);
  } else {
    jsonHeader(connData, 404);
    return HTTPD_CGI_DONE;
  }
}
//#endif // ARTNET
