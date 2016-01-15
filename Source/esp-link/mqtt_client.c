#ifdef MQTT
#include <esp8266.h>
#include "cgiwifi.h"
#include "config.h"
#include "mqtt.h"
#include "pwm.h"

#ifdef MQTTCLIENT_DBG
#define DBG_MQTTCLIENT(format, ...) os_printf(format, ## __VA_ARGS__)
#else
#define DBG_MQTTCLIENT(format, ...) do { } while(0)
#endif

MQTT_Client mqttClient; // main mqtt client used by esp-link

#ifdef BRUNNELS
char* statusTopicStr;
static char* onlineMsgStr;
#endif

static MqttCallback connected_cb;
static MqttCallback disconnected_cb;
static MqttCallback published_cb;
static MqttDataCallback data_cb;

void ICACHE_FLASH_ATTR
mqttConnectedCb(uint32_t *args) {
  DBG_MQTTCLIENT("MQTT Client: Connected\n");
  MQTT_Client* client = (MQTT_Client*)args;
  //MQTT_Subscribe(client, "system/time", 0); // handy for testing

#ifdef PWMOUT
  for (uint8_t i=0; i<PWM_CHANNEL; i++) {
    MQTT_Subscribe(client, flashConfig.mqtt_pwms[i], 0);
  }
#endif

#ifdef BRUNNELS
  MQTT_Publish(client, "announce/all", onlineMsgStr, 0, 0);
#endif
  if (connected_cb)
    connected_cb(args);
}

void ICACHE_FLASH_ATTR
mqttDisconnectedCb(uint32_t *args) {
//  MQTT_Client* client = (MQTT_Client*)args;
  DBG_MQTTCLIENT("MQTT Client: Disconnected\n");
  if (disconnected_cb)
    disconnected_cb(args);
}

void ICACHE_FLASH_ATTR
mqttPublishedCb(uint32_t *args) {
//  MQTT_Client* client = (MQTT_Client*)args;
  DBG_MQTTCLIENT("MQTT Client: Published\n");
  if (published_cb)
    published_cb(args);
}

static int ICACHE_FLASH_ATTR
mqttPwmData(const char* const topic, const uint32_t topic_len, const char* const data, const uint32_t data_len) {
    /* has to be between 1 and 3 digits (0..100) */
    if (data_len < 1 || data_len > 3) {
        PWRN(PWMOUT_LOGL, "data with wrong size");
        return -1;
    }

    uint16_t channel = 0xFFFF;
    for (uint8_t i=0; i<PWM_CHANNEL; i++) {
        if (strlen(flashConfig.mqtt_pwms[i]) == topic_len && memcmp(flashConfig.mqtt_pwms[i], topic, topic_len) == 0) {
            channel = i;
            break;
        }
    }


    if (channel >= PWM_CHANNEL) {
        PWRN(PWMOUT_LOGL, "Higher channel than PWM output number");
        return -2;
    }

    /* append trailing zero */
    char data_buf[data_len + 1];
    memcpy(data_buf, data, data_len);
    data_buf[data_len] = 0x00;

    const uint8_t data_value = atoi(data_buf);
    if (data_value > 100) {
        PWRN(PWMOUT_LOGL, "data value out of range");
        return -3;
    }

    /* from percent to uint8_t (0..255) */
    const uint16_t duty = data_value * 255 / 100;
    if ( pwm_set_duty(duty, channel) ) {
        /* only update, if value has realy changed */
        pwm_start();
    }

    PDBG(PWMOUT_LOGL, "PWM output %u changed to %u", channel, duty);
    return 1;
}

void ICACHE_FLASH_ATTR
mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len) {
  //  MQTT_Client* client = (MQTT_Client*)args;

#ifdef MQTTCLIENT_DBG
  char *topicBuf = (char*)os_zalloc(topic_len + 1);
  char *dataBuf = (char*)os_zalloc(data_len + 1);

  os_memcpy(topicBuf, topic, topic_len);
  topicBuf[topic_len] = 0;

  os_memcpy(dataBuf, data, data_len);
  dataBuf[data_len] = 0;

  os_printf("MQTT Client: Received topic: %s, data: %s\n", topicBuf, dataBuf);
  os_free(topicBuf);
  os_free(dataBuf);
#endif

#ifdef PWMOUT
    mqttPwmData(topic, topic_len, data, data_len);
#endif

  if (data_cb)
    data_cb(args, topic, topic_len, data, data_len);
}

void ICACHE_FLASH_ATTR
wifiStateChangeCb(uint8_t status)
{
  if (flashConfig.mqtt_enable) {
    if (status == wifiGotIP && mqttClient.connState != TCP_CONNECTING) {
      MQTT_Connect(&mqttClient);
    }
    else if (status == wifiIsDisconnected && mqttClient.connState == TCP_CONNECTING) {
      MQTT_Disconnect(&mqttClient);
    }
  }
}

void ICACHE_FLASH_ATTR
mqtt_client_init()
{
  MQTT_Init(&mqttClient, flashConfig.mqtt_host, flashConfig.mqtt_port, 0, flashConfig.mqtt_timeout,
    flashConfig.mqtt_clientid, flashConfig.mqtt_username, flashConfig.mqtt_password,
    flashConfig.mqtt_keepalive);

// removed client_id concat for now until a better solution is devised
//      statusTopicStr = (char*)os_zalloc(strlen(flashConfig.mqtt_clientid) + strlen(flashConfig.mqtt_status_topic) + 2);
//      os_strcpy(statusTopicStr, flashConfig.mqtt_clientid);
//      os_strcat(statusTopicStr, "/");

#ifdef BRUNNELS
    char* onlineMsg = " is online";
    onlineMsgStr = (char*)os_zalloc(strlen(flashConfig.mqtt_clientid) + strlen(onlineMsg) + 1);
    os_strcpy(onlineMsgStr, flashConfig.mqtt_clientid);
    os_strcat(onlineMsgStr, onlineMsg);

    char* offlineMsg = " is offline";
    char* offlineMsgStr = (char*)os_zalloc(strlen(flashConfig.mqtt_clientid) + strlen(offlineMsg) + 1);
    os_strcpy(offlineMsgStr, flashConfig.mqtt_clientid);
    os_strcat(offlineMsgStr, offlineMsg);

    char* lwt = "/lwt";
    char *lwtMsgStr = (char*)os_zalloc(strlen(flashConfig.mqtt_clientid) + strlen(lwt) + 1);
    os_strcpy(lwtMsgStr, flashConfig.mqtt_clientid);
    os_strcat(lwtMsgStr, lwt);
    MQTT_InitLWT(&mqttClient, lwtMsgStr, offlineMsg, 0, 0);
#endif

  MQTT_OnConnected(&mqttClient, mqttConnectedCb);
  MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);
  MQTT_OnPublished(&mqttClient, mqttPublishedCb);
  MQTT_OnData(&mqttClient, mqttDataCb);

  if (flashConfig.mqtt_enable && strlen(flashConfig.mqtt_host) > 0)
    MQTT_Connect(&mqttClient);

  wifiAddStateChangeCb(wifiStateChangeCb);
}

void ICACHE_FLASH_ATTR
mqtt_client_on_connected(MqttCallback connectedCb) {
  connected_cb = connectedCb;
}

void ICACHE_FLASH_ATTR
mqtt_client_on_disconnected(MqttCallback disconnectedCb) {
  disconnected_cb = disconnectedCb;
}

void ICACHE_FLASH_ATTR
mqtt_client_on_published(MqttCallback publishedCb) {
  published_cb = publishedCb;
}

void ICACHE_FLASH_ATTR
mqtt_client_on_data(MqttDataCallback dataCb) {
  data_cb = dataCb;
}

#endif // MQTT
