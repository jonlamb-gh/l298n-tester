/**
 * @file driver.c
 * @brief TODO.
 *
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "board_def.h"
#include "pwm.h"
#include "adc.h"
#include "driver.h"

#define CS_MUX (ADC_MUX2)

static driver_s g_state;

static void init_state(void)
{
    g_state.in1 = 0;
    g_state.in2 = 0;
    g_state.en = 0;
    g_state.cs = 0;
    g_state.pwm_duty = 0;
    g_state.pwm_period = PWM_DEFAULT_PERIOD;
}

void driver_init(void)
{
    pwm_init();

    pwm_disable();

    bit_clear(PWM_PORT, BIT(PWM_BIT));

    bit_set(DRIVER_IO_DDR, BIT(DRIVER_IO_BIT_IN1) | BIT(DRIVER_IO_BIT_IN2));

    bit_clear(DRIVER_IO_PORT, BIT(DRIVER_IO_BIT_IN1) | BIT(DRIVER_IO_BIT_IN2));

    adc_init_pins(DRIVER_AIN_BIT_CS);

    init_state();
}

void driver_get_state(
        driver_s * const state)
{
    g_state.cs = adc_read(CS_MUX);

    state->in1 = g_state.in1;
    state->in2 = g_state.in2;
    state->en = g_state.en;
    state->cs = g_state.cs;
    state->pwm_duty = g_state.pwm_duty;
    state->pwm_period = g_state.pwm_period;
}

void driver_set_direction(
        const uint8_t in1,
        const uint8_t in2)
{
    g_state.in1 = in1;
    g_state.in2 = in2;

    if(in1 == 0)
    {
        bit_clear(DRIVER_IO_PORT, BIT(DRIVER_IO_BIT_IN1));
    }
    else
    {
        bit_set(DRIVER_IO_PORT, BIT(DRIVER_IO_BIT_IN1));
    }

    if(in2 == 0)
    {
        bit_clear(DRIVER_IO_PORT, BIT(DRIVER_IO_BIT_IN2));
    }
    else
    {
        bit_set(DRIVER_IO_PORT, BIT(DRIVER_IO_BIT_IN2));
    }
}

void driver_set_en(
        const uint16_t en)
{
    g_state.en = en;
    g_state.pwm_duty = en;

    pwm_enable(en);
}
