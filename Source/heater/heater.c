#include <esp8266.h>
#include "heater.h"

/* Defines the GPIOs of the ESP which are connected with the shaft encoder
 * of the heater
 */
#define SHAFT_ENCODER_PIN1	0
#define SHAFT_ENCODER_PIN2	2


#define DELAY_IN_US			150000
#define MIN_TEMPERATURE		7
#define MAX_TEMPERATURE		30
#define TASK_QUEUE_LEN		1


uint8_t _currentTemperature = 0;
//uint8_t _lastOnTemperature = 20;

static int8_t nextEncoderSteps = 0;

static void ICACHE_FLASH_ATTR heater_task(ETSEvent* const e) {
	static int8_t leftEncoderSteps = 0;
	/* get next encoder steps, if last was finished */
	if (leftEncoderSteps == 0) {
		leftEncoderSteps = nextEncoderSteps;
		nextEncoderSteps = 0;
	}

	/* stop task, if nothing to do */
	if (leftEncoderSteps == 0) {
		return;
	}

	PDBG(HEATER_LOGL, "Changing %d ...", leftEncoderSteps);

	uint8_t pin1 = 0;
	uint8_t pin2 = 0;
	if (leftEncoderSteps > 0) {
		pin1 = SHAFT_ENCODER_PIN1;
		pin2 = SHAFT_ENCODER_PIN2;
		leftEncoderSteps--;
	} else {
		pin1 = SHAFT_ENCODER_PIN2;
		pin2 = SHAFT_ENCODER_PIN1;
		leftEncoderSteps++;
	}

	GPIO_OUTPUT_SET(pin1, 0);
	os_delay_us(DELAY_IN_US);
	GPIO_OUTPUT_SET(pin2, 0);
	os_delay_us(DELAY_IN_US);
	GPIO_OUTPUT_SET(pin1, 1);
	os_delay_us(DELAY_IN_US);
	GPIO_OUTPUT_SET(pin2, 1);
	os_delay_us(DELAY_IN_US);

	/* start task again, if there is somthing to do */
	if (leftEncoderSteps != 0) {
		system_os_post(USER_TASK_PRIO_0, 0, 0);
	}
}


static void ICACHE_FLASH_ATTR _shaftEncoderSteps(const int8_t steps)
{
	nextEncoderSteps += steps;
	if ( !system_os_post(USER_TASK_PRIO_0, 0, 0) ) {
		PERR(HEATER_LOGL, "New temperature not send to heater task");
	}
}


static void ICACHE_FLASH_ATTR _saveTemp(const uint8_t newTemp)
{
	if (newTemp < MIN_TEMPERATURE && newTemp > 0) {
		_currentTemperature = MIN_TEMPERATURE;
	} else if (newTemp > MAX_TEMPERATURE){
		_currentTemperature = MAX_TEMPERATURE;
	} else {
		_currentTemperature = newTemp;
	}
}


//void ICACHE_FLASH_ATTR heater_incTemp(void)
//{
//	_shaftEncoderStep(true);
//	_saveTemp(_currentTemperature + 1);
//}


//void ICACHE_FLASH_ATTR heater_decTemp(void)
//{
//	_shaftEncoderStep(false);
//	_saveTemp(_currentTemperature - 1);
//}


void ICACHE_FLASH_ATTR heater_off()
{
//	/* save current temperature for switching on again */
//	_lastOnTemperature = _currentTemperature;

	/*
	 * switch fully off to reset possibly temperature offsets
	 * between Sparmatic and _currentTemperature
	 */
	_shaftEncoderSteps(-MAX_TEMPERATURE);

	_currentTemperature = 0;
}



void ICACHE_FLASH_ATTR heater_setTemp(const uint8_t temperature)
{
	/* switch off, if temperature is to small */
	if (temperature < MIN_TEMPERATURE) {
		heater_off();
		return;
	}

	PDBG(HEATER_LOGL, "Setting temperature to %u ...", temperature);

	const bool incTemp = (temperature > _currentTemperature);
	uint8_t diff = 0;
	if (incTemp) {
		diff = temperature - _currentTemperature;
	} else {
		diff = _currentTemperature - temperature;
	}

	/*
	 * remove offset,
	 * if diff includes switch on temperature (MIN_TEMPERATURE).
	 * The value of _currentTemperature always has to be between
	 * MIN_TEMPERATURE and MAX_TEMPERATURE or 0
	 */
	if (_currentTemperature < MIN_TEMPERATURE) {
		diff -= MIN_TEMPERATURE;
	}

	if (incTemp) {
		_shaftEncoderSteps(diff);
	} else {
		_shaftEncoderSteps(-diff);
	}

	_saveTemp(temperature);
}


//void ICACHE_FLASH_ATTR heater_restore()
//{
//	heater_setTemp(_lastOnTemperature);
//}

void ICACHE_FLASH_ATTR heater_init(void)
{
	/* use inverted logic, because of PNP transistor */
	/* simulate open state for shaft encoder */
	GPIO_OUTPUT_SET(SHAFT_ENCODER_PIN1, 1);
	GPIO_OUTPUT_SET(SHAFT_ENCODER_PIN2, 1);

	static os_event_t taskQueue[TASK_QUEUE_LEN];
	if ( !system_os_task(heater_task, USER_TASK_PRIO_0, taskQueue, TASK_QUEUE_LEN) ) {
		PERR(HEATER_LOGL, "Task creation failed");
	}
	/* switch sparmatic off after reset
	 * is needed to deteckt the correct offset
	 */
	heater_setTemp(0);
}
