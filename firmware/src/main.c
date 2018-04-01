/**
 * @file main.c
 * @brief Main.
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>

#include "board_def.h"
#include "time.h"
#include "pwm.h"
#include "adc.h"
#include "input.h"
#include "protocol.h"
#include "transport.h"

static void wait_for_transport(void)
{
    time_delay_ms(1000);

    while(transport_ready() == 0)
    {
        led_toggle();
        time_delay_ms(50);
    }

    led_on();
    time_delay_ms(1000);
    led_off();
}

int main(void)
{
    input_s input;

    disable_interrupt();

    cpu_prescale(CPU_16MHZ);

    led_init();
    led_off();

    time_init();

    pwm_init();

    input_init();

    enable_interrupt();

    transport_init();

    wait_for_transport();

    while(1)
    {
        time_delay_ms(1000);

        led_toggle();

        input_update(&input);
    }

    return 0;
}
