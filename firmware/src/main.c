/**
 * @file main.c
 * @brief Main.
 *
 */

#include <string.h>
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

static void init_msg(
        const uint16_t error_cnt,
        proto_msg_s * const msg)
{
    (void) memset(msg, 0, sizeof(*msg));
    msg->preamble = PROTO_MSG_PREAMBLE;
    msg->error_cnt = error_cnt;
}

int main(void)
{
    proto_msg_s msg;

    wdt_disable();
    disable_interrupt();

    cpu_prescale(CPU_16MHZ);

    led_init();
    led_off();

    time_init();

    driver_init();

    input_init();

    enable_interrupt();

    transport_init();

    wait_for_transport();

    init_msg(0, &msg);

    driver_set_en(600);

    while(1)
    {
        time_delay_ms(500);

        led_toggle();

        msg.cnt += 1;
        msg.start_time = time_get_ms();

        input_update(&msg.input_state);

        driver_get_state(&msg.driver_state);

        msg.checksum = protocol_crc16(&msg);

        const uint8_t err = transport_send(&msg);

        if(err != 0)
        {
            msg.error_cnt += 1;
        }
    }

    return 0;
}
