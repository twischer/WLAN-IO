/*
 *  Example read temperature and humidity from DHT22
 *  
 *  https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf
 *  https://www.adafruit.com/datasheets/Digital%20humidity%20and%20temperature%20sensor%20AM2302.pdf
 * 
 *  For a single device, connect as follows:
 *  DHT22 1 (Vcc) to Vcc (3.3 Volts)
 *  DHT22 2 (DATA_OUT) to ESP GPIO (see Pin number)
 *  DHT22 3 (NC)
 *  DHT22 4 (GND) to GND
 *
 *  Pin number:
 *  -----------
 *  Pin 0 = GPIO16
 *  Pin 1 = GPIO5
 *  Pin 2 = GPIO4
 *  Pin 3 = GPIO0
 *  Pin 4 = GPIO2
 *  Pin 5 = GPIO14
 *  Pin 6 = GPIO12
 *  Pin 7 = GPIO13
 *  Pin 8 = GPIO15
 *  Pin 9 = GPIO3
 *  Pin 10 = GPIO1
 *  Pin 11 = GPIO9
 *  Pin 12 = GPIO10
 *
 * Between Vcc and DATA_OUT need to connect a pull-up resistor of 5 kOh.
 *
 */

#include <esp8266.h>
#include "dhtxx_mqtt.h"
#include "dht22.h"
#include "gpio16.h"
#include "mqtt_client.h"

/* DHT refresh rate (in ms) */
#define DELAY 60*1000 /* each minute */

extern uint8_t pin_num[GPIO_PIN_NUM];

LOCAL os_timer_t dht22_timer;

static DHT_Sensor sensor;

LOCAL void ICACHE_FLASH_ATTR dht22_cb(void *arg)
{
	DHT_Sensor_Data data;
	os_timer_disarm(&dht22_timer);

	// One DHT22 sensor
	if (DHTRead(&sensor, &data))
	{
		char buff[20];
		DHTFloat2String(buff, data.temperature);

		PDBG(DHTXX_LOGL, "Temperature: %s *C\r\n", buff);
		MQTT_Publish(&mqttClient, "temp", buff, 0, 1);

		DHTFloat2String(buff, data.humidity);
		PDBG(DHTXX_LOGL, "Humidity: %s %%\r\n", buff);
		MQTT_Publish(&mqttClient, "humi", buff, 0, 1);
	} else {
		PWRN(DHTXX_LOGL, "Failed to read temperature and humidity sensor");
		MQTT_Publish(&mqttClient, "temp", "0", 0, 1);
		MQTT_Publish(&mqttClient, "humi", "0", 0, 1);
	}
	os_timer_arm(&dht22_timer, DELAY, 1);
}



void dhtxx_mqtt_init(void)
{
	/*
	 *  Pin number:
	 *  -----------
	 *  Pin 0 = GPIO16
	 *  Pin 1 = GPIO5
	 *  Pin 2 = GPIO4
	 *  Pin 3 = GPIO0
	 *  Pin 4 = GPIO2
	 *  Pin 5 = GPIO14
	 *  Pin 6 = GPIO12
	 *  Pin 7 = GPIO13
	 *  Pin 8 = GPIO15
	 *  Pin 9 = GPIO3
	 *  Pin 10 = GPIO1
	 *  Pin 11 = GPIO9
	 *  Pin 12 = GPIO10
	 */

	// One DHT22 sensor
	sensor.pin = 9;
	sensor.type = DHT22;
	PINF(DHTXX_LOGL, "DHT22 init on GPIO%d\r\n", pin_num[sensor.pin]);
	DHTInit(&sensor);


	os_timer_disarm(&dht22_timer);
	os_timer_setfn(&dht22_timer, (os_timer_func_t *)dht22_cb, (void *)0);
	os_timer_arm(&dht22_timer, DELAY, 1);
}
