#include "zcd.h"
#include "pwm.h"

#ifdef ZCD_DBG
#define DBG(format, ...) os_printf(format, ## __VA_ARGS__)
#else
#define DBG(format, ...) do { } while(0)
#endif

void zcd_interrupt(uint32 intr_mask, void *arg) {
//    uint32 gpio_status;
//    gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
//    //clear interrupt status
//    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);

    const bool positive_edge = gpio_input_get() & BIT(ZCD_IO_NUM);
    gpio_intr_ack(intr_mask);

    /* only process the zero crossing detection interrupt */
    if ( (intr_mask & BIT(ZCD_IO_NUM)) == 0 ) {
         return;
    }

    static uint32 last_timestamp = 0;
    const uint32 new_time = system_get_time();
    const uint32 diff = new_time - last_timestamp;
    last_timestamp = new_time;

    DBG("Interrupt edge %u, diff %u\n", positive_edge, diff);

    // TODO use any edge and calulate the middle for syncing,
    // becasue the positive edge will be detected a little bit later
    // than the negativ edge,
    // becasue of the input high recognition of till 2 Volt

    /* only sync the PWM, if the freuency is nearly the ??erwartet?? frequency */
    if (diff > ZCD_INTERVAL_MIN && diff < ZCD_INTERVAL_MAX) {
        pwm_sync();
        DBG("PWM synchronized to power line frequency\n");
    }
}


void ICACHE_FLASH_ATTR zcd_init()
{
    PIN_FUNC_SELECT(ZCD_IO_MUX, ZCD_IO_FUNC);
    GPIO_DIS_OUTPUT(ZCD_IO_NUM);
    PIN_PULLUP_DIS(ZCD_IO_MUX);

    ETS_GPIO_INTR_DISABLE();
    gpio_register_set(GPIO_PIN_ADDR(GPIO_ID_PIN(ZCD_IO_NUM)), GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_POSEDGE)
                | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE)
                | GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));

    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(ZCD_IO_NUM));
    //enable interrupt
    gpio_pin_intr_state_set(GPIO_ID_PIN(ZCD_IO_NUM), GPIO_PIN_INTR_POSEDGE);

    gpio_intr_handler_register(zcd_interrupt, NULL);
    ETS_GPIO_INTR_ENABLE();
}
