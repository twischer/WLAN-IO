#ifndef __PWM_H__
#define __PWM_H__

#include "ets_sys.h"
#include "user_config.h"


struct pwm_single_param {
    uint16 gpio_set;
    uint16 gpio_clear;
    uint16 h_time;
};

struct pwm_param {
    uint16 period;
    uint16 freq;
    uint8  duty[PWM_CHANNEL];
};

#define PWM_DEPTH 255

#define PWM_1S 1000000

#ifdef ESP03
/* pin out for ESP03.
 * (14 soldering points, 9 GPIOs)
 * Wiring of schematics/dimmerWall.sch is used here.
 */
#define PWM_0_OUT_IO_MUX PERIPHS_IO_MUX_MTDO_U
#define PWM_0_OUT_IO_NUM 15
#define PWM_0_OUT_IO_FUNC  FUNC_GPIO15
#else
/* pin out for ESP02.
 * (2x8 pin header, 4 GPIOs)
 */
#define PWM_0_OUT_IO_MUX PERIPHS_IO_MUX_GPIO0_U
#define PWM_0_OUT_IO_NUM 0
#define PWM_0_OUT_IO_FUNC  FUNC_GPIO0

#define PWM_1_OUT_IO_MUX PERIPHS_IO_MUX_GPIO2_U
#define PWM_1_OUT_IO_NUM 2
#define PWM_1_OUT_IO_FUNC  FUNC_GPIO2

#define PWM_2_OUT_IO_MUX PERIPHS_IO_MUX_U0RXD_U
#define PWM_2_OUT_IO_NUM 3
#define PWM_2_OUT_IO_FUNC  FUNC_GPIO3
#endif

void pwm_init(uint16 freq, uint8 *duty);
void pwm_start(void);

bool pwm_set_duty(uint8 duty, uint8 channel);
uint8 pwm_get_duty(uint8 channel);
void pwm_set_freq(uint16 freq);
uint16 pwm_get_freq(void);

void pwm_sync();
#endif


