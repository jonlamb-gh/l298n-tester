/**
 * @file procedure_1.c
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
#include "procedure_1.h"

static int32_t map_i32(
        const int32_t x,
        const int32_t in_min,
        const int32_t in_max,
        const int32_t out_min,
        const int32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void procedure_1_run(
        proto_msg_s * const msg)
{
    uint8_t toggle_dir;
    uint16_t pwm_duty;
    uint32_t pwm_period;

    led_on();

    toggle_dir = 0;
    msg_init(msg->error_cnt, msg);
    msg->running_proc = 1;
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
                1, // 1000 kHz
                100UL); // 10 kHz

        driver_get_state(&msg->driver_state);
        msg->driver_state.pwm_duty = pwm_duty;
        msg->driver_state.pwm_period = pwm_period;

        msg_send(msg);

        if(msg->input_state.bt0 != 0)
        {
            toggle_dir = !toggle_dir;
        }

        if(msg->input_state.bt2 != 0)
        {
            driver_set_direction(1, 0);
            msg->start_time = time_get_ms();

            if(toggle_dir != 0)
            {
                driver_toggle_direction();
            }

            driver_enable(pwm_duty, pwm_period);

            do
            {
                led_toggle();

                driver_get_state(&msg->driver_state);

                msg_send(msg);

                input_update(INPUT_GROUP_BUTTON, &msg->input_state);
            }
            while((msg->input_state.bt2 != 0) && (msg->input_state.bt1 !=0));

            led_on();
            driver_disable();
            driver_set_direction(0, 0);

            msg->end_time = time_get_ms();

            msg_send(msg);
        }

        time_delay_ms(20);
    }
    while(msg->input_state.bt1 != 0);

    driver_disable();
    driver_set_direction(0, 0);

    msg->end_time = time_get_ms();
    driver_get_state(&msg->driver_state);

    msg_send(msg);

    led_off();

    time_delay_ms(1000);
}
