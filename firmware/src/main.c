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

// pwm frequency
// https://www.pjrc.com/teensy/td_pulse.html
// defaults for teensy++ 2.0
// 0 - 976.56 Hz
// 1 - 3921.57 Hz
// 2 - 3921.57 Hz
// 3 - 3921.57 Hz

int main(void)
{
    disable_interrupt();

    cpu_prescale(CPU_16MHZ);

    led_init();
    led_off();

    time_init();

    enable_interrupt();

    while(1)
    {
        time_delay_ms(1000);

        led_toggle();
    }

    return 0;
}
