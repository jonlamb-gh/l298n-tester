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

static void init_msg(
        const uint16_t error_cnt)
{
    (void) memset(&tx_msg, 0, sizeof(tx_msg));
    tx_msg.preamble = PROTO_MSG_PREAMBLE;
    tx_msg.error_cnt = error_cnt;
}

static void send_msg(void)
{
    tx_msg.cnt += 1;

    tx_msg.checksum = protocol_crc16(&tx_msg);

    const uint8_t err = transport_send(&tx_msg);

    if(err != 0)
    {
        tx_msg.error_cnt += 1;
    }
}

static void test_routine(void)
{
    led_on();

    init_msg(tx_msg.error_cnt);

    tx_msg.start_time = time_get_ms();

    // TODO
    //driver_configure(period, duty);

    // TODO - toggle mechanism?
    driver_set_direction(
            tx_msg.input_state.bt0,
            tx_msg.input_state.bt1);

    // TODO - pt1/pt0
    driver_set_en(500);

    do
    {
        input_update(&tx_msg.input_state);

        driver_get_state(&tx_msg.driver_state);

        time_delay_ms(20);

        send_msg();
    }
    while(tx_msg.input_state.bt2 != 0);

    driver_set_en(0);

    tx_msg.end_time = time_get_ms();

    driver_get_state(&tx_msg.driver_state);

    send_msg();

    led_off();
}

int main(void)
{
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

    init_msg(0);

    while(1)
    {
        // pt0/pt1 -> period/duty

        // direction?

        // bt0 -> in1 ?
        // bt1 -> in2 ?

        // bt2 -> on/off

        led_off();

        input_update(&tx_msg.input_state);

        if(tx_msg.input_state.bt2 != 0)
        {
            led_on();

            test_routine();
        }
    }

    return 0;
}
