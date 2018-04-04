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
    // TODO - pt1/pt0
    // make one of them use a logarithmic pot. for pwm_period
    const uint16_t pwm_duty = 500;
    const uint32_t pwm_period = 400;

    led_on();

    init_msg(tx_msg.error_cnt);

    tx_msg.start_time = time_get_ms();

    // TODO - toggle mechanism?
    driver_set_direction(
            tx_msg.input_state.bt0,
            tx_msg.input_state.bt1);

    driver_set_pwm(pwm_duty, pwm_period);

    driver_enable(1);

    do
    {
        input_update(&tx_msg.input_state);

        driver_get_state(&tx_msg.driver_state);

        time_delay_ms(20);

        send_msg();

        led_toggle();
    }
    while(tx_msg.input_state.bt2 != 0);

    led_on();

    driver_enable(0);

    driver_set_direction(0, 0);

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
