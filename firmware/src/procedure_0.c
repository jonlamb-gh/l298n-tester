/**
 * @file procedure_0.c
 * @brief TODO.
 *
 */

#include <stdint.h>
#include <avr/io.h>

#include "board_def.h"
#include "time.h"
#include "adc.h"
#include "pwm.h"
#include "input.h"
#include "driver.h"
#include "protocol.h"
#include "transport.h"
#include "msg.h"
#include "procedure_0.h"

static int32_t map_i32(
        const int32_t x,
        const int32_t in_min,
        const int32_t in_max,
        const int32_t out_min,
        const int32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void procedure_0_run(
        proto_msg_s * const msg)
{
    uint16_t pwm_duty;

    // fixed PWM period
    const uint32_t pwm_period = 5;

    led_on();

    msg_init(msg->error_cnt, msg);

    msg->running_proc = 0;

    // read inputs while pressed
    do
    {
        input_update(INPUT_ALL, &msg->input_state);

        pwm_duty = (uint16_t) map_i32(
                msg->input_state.pt1,
                0,
                ADC_VALUE_MAX,
                0,
                PWM_DUTY_MAX);

        msg->driver_state.delay_interval = msg->input_state.pt0;

        driver_get_state(&msg->driver_state);
        msg->driver_state.pwm_duty = pwm_duty;

        msg_send(msg);

        time_delay_ms(20);
    }
    while(msg->input_state.bt0 != 0);

    time_delay_ms(500);

    msg->start_time = time_get_ms();

    driver_set_direction(0, 1);

    driver_enable(pwm_duty, pwm_period);

    do
    {
        input_update(
                INPUT_GROUP_BUTTON,
                &msg->input_state);

        driver_get_state(&msg->driver_state);

        msg_send(msg);

        led_toggle();

        driver_toggle_direction();

        time_delay_ms(msg->driver_state.delay_interval);
    }
    while((msg->input_state.bt1 == 0) && (msg->input_state.bt2 == 0));

    driver_disable();

    driver_set_direction(0, 0);

    msg->end_time = time_get_ms();

    driver_get_state(&msg->driver_state);

    msg_send(msg);

    led_off();

    time_delay_ms(1000);
}
