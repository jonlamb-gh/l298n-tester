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
#include "driver.h"
#include "protocol.h"
#include "transport.h"
#include "msg.h"
#include "procedure_0.h"
#include "procedure_1.h"
#include "procedure_2.h"

#ifndef F_CPU
#error "F_CPU not defined"
#endif

// embed F_CPU/mmcu in the ELF for debugging/simulation
#ifdef DEBUG_BUILD
#include "avr/avr_mcu_section.h"
AVR_MCU(F_CPU, "at90usb1286");
#endif

static proto_msg_s tx_msg;

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
    wdt_disable();
    disable_interrupt();
    wdt_enable(WDTO_8S);

    cpu_prescale(CPU_16MHZ);

    led_init();
    led_off();

    time_init();

    driver_init();

    input_init();

    enable_interrupt();

    transport_init();

    wait_for_transport();

    msg_init(0, &tx_msg);

    while(1)
    {
        wdt_reset();

        input_update(INPUT_ALL, &tx_msg.input_state);

        if(tx_msg.input_state.bt0 != 0)
        {
            procedure_0_run(&tx_msg);
        }
        else if(tx_msg.input_state.bt1 != 0)
        {
            procedure_1_run(&tx_msg);
        }
        else if(tx_msg.input_state.bt2 != 0)
        {
            procedure_2_run(&tx_msg);
        }

        tx_msg.running_proc = 0xFF;

        // periodically send out a message
        if(time_get_and_clear_event() != 0)
        {
            led_toggle();
            tx_msg.start_time = time_get_ms();
            tx_msg.end_time = tx_msg.start_time;
            driver_get_state(&tx_msg.driver_state);
            msg_send(&tx_msg);
        }
    }

    return 0;
}
