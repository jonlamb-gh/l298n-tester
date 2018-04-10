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

#ifndef F_CPU
#error "F_CPU not defined"
#endif

// embed F_CPU/mmcu in the ELF for debugging/simulation
#ifdef DEBUG_BUILD
#include "avr/avr_mcu_section.h"
AVR_MCU(F_CPU, "at90usb1286");
#endif

static proto_msg_s tx_msg;

static int32_t map_i32(
        const int32_t x,
        const int32_t in_min,
        const int32_t in_max,
        const int32_t out_min,
        const int32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

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

static void run_test_procedure(void)
{
    uint16_t pwm_duty;

    // fixed PWM period
    const uint32_t pwm_period = 40;

    led_on();

    init_msg(tx_msg.error_cnt);

    // read inputs while pressed
    do
    {
        input_update(INPUT_ALL, &tx_msg.input_state);

        pwm_duty = (uint16_t) map_i32(
                tx_msg.input_state.pt1,
                0,
                ADC_VALUE_MAX,
                0,
                PWM_DUTY_MAX);

        tx_msg.driver_state.delay_interval = tx_msg.input_state.pt0;

        driver_get_state(&tx_msg.driver_state);
        tx_msg.driver_state.pwm_duty = pwm_duty;

        send_msg();

        time_delay_ms(20);
    }
    while(tx_msg.input_state.bt2 != 0);

    time_delay_ms(500);

    tx_msg.start_time = time_get_ms();

    driver_set_direction(0, 1);

    driver_enable(pwm_duty, pwm_period);

    do
    {
        input_update(
                INPUT_GROUP_BUTTON,
                &tx_msg.input_state);

        driver_get_state(&tx_msg.driver_state);

        send_msg();

        led_toggle();

        driver_toggle_direction();

        time_delay_ms(tx_msg.driver_state.delay_interval);
    }
    while(tx_msg.input_state.bt1 == 0);

    driver_disable();

    driver_set_direction(0, 0);

    tx_msg.end_time = time_get_ms();

    driver_get_state(&tx_msg.driver_state);

    send_msg();

    led_off();

    time_delay_ms(500);
}

/*
static void test_routine(void)
{
    led_on();

    init_msg(tx_msg.error_cnt);

    // TODO - flags for groups?
    input_update(INPUT_ALL, &tx_msg.input_state);

    // pt0 is logarithmic, mapped to pwm_period
    // pt1 is linear, mapped to pwm_duty
    const uint16_t pwm_duty = (uint16_t) map_i32(
            tx_msg.input_state.pt1,
            0,
            ADC_VALUE_MAX,
            0,
            PWM_DUTY_MAX);

    // TODO - solder things up differently
    const uint32_t pwm_period = (uint32_t) map_i32(
            tx_msg.input_state.pt0,
            0,
            ADC_VALUE_MAX,
            1, // 1000 kHz
            100UL); // 10 kHz

    tx_msg.start_time = time_get_ms();

    driver_set_direction(
            tx_msg.input_state.bt0,
            tx_msg.input_state.bt1);

    driver_enable(pwm_duty, pwm_period);

    do
    {
        input_update(INPUT_ALL, &tx_msg.input_state);

        driver_get_state(&tx_msg.driver_state);

        send_msg();

        led_toggle();
    }
    while(tx_msg.input_state.bt2 != 0);

    led_on();

    driver_disable();

    driver_set_direction(0, 0);

    tx_msg.end_time = time_get_ms();

    driver_get_state(&tx_msg.driver_state);

    send_msg();

    led_off();
}
*/

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
        input_update(INPUT_ALL, &tx_msg.input_state);

        if(tx_msg.input_state.bt2 != 0)
        {
            run_test_procedure();
        }

        // periodically send out a message
        if(time_get_and_clear_event() != 0)
        {
            led_toggle();
            tx_msg.start_time = time_get_ms();
            tx_msg.end_time = tx_msg.start_time;
            driver_get_state(&tx_msg.driver_state);
            send_msg();
        }
    }

    return 0;
}
