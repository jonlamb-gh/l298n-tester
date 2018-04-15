/**
 * @file procedure_2.c
 * @brief TODO.
 *
 */

#include <stdint.h>
#include <avr/io.h>

#include "board_def.h"
#include "math.h"
#include "time.h"
#include "adc.h"
#include "pwm.h"
#include "input.h"
#include "driver.h"
#include "protocol.h"
#include "transport.h"
#include "msg.h"
#include "procedure_1.h"

static void pwm_interrupt_cb(
        const uint8_t top)
{
    if(top == 0)
    {
        led_on();
    }
    else
    {
        led_off();
    }
}

void procedure_2_run(
        proto_msg_s * const msg)
{
    uint16_t pwm_duty;
    uint32_t pwm_period;

    led_off();

    msg_init(msg->error_cnt, msg);
    msg->running_proc = 2;
    msg->driver_state.delay_interval = 0;

    msg->start_time = time_get_ms();

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

        // TODO - re-solder to get high end of the log pot to match high end value
        pwm_period = (uint32_t) map_i32(
                msg->input_state.pt0,
                0,
                ADC_VALUE_MAX,
                1,
                1000UL);

        driver_get_state(&msg->driver_state);
        msg->driver_state.pwm_duty = pwm_duty;
        msg->driver_state.pwm_period = pwm_period;

        msg_send(msg);

        if(msg->input_state.bt1 != 0)
        {
            driver_set_direction(1, 0);
            msg->start_time = time_get_ms();

            driver_enable(pwm_duty, pwm_period);
            pwm_set_interrupt_callback(&pwm_interrupt_cb);

            do
            {
                driver_get_state(&msg->driver_state);

                msg_send(msg);

                input_update(INPUT_ALL, &msg->input_state);

                time_delay_ms(20);
            }
            while((msg->input_state.bt0 != 0) && (msg->input_state.bt2 !=0));

            led_off();
            driver_disable();
            driver_set_direction(0, 0);

            msg->end_time = time_get_ms();

            msg_send(msg);

            time_delay_ms(100);
        }

        time_delay_ms(20);
    }
    while(msg->input_state.bt2 != 0);

    driver_disable();
    driver_set_direction(0, 0);
    pwm_set_interrupt_callback(NULL);

    msg->end_time = time_get_ms();
    driver_get_state(&msg->driver_state);

    msg_send(msg);

    led_off();

    time_delay_ms(1000);
}
