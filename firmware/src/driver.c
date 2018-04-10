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
#include "time.h"
#include "driver.h"

#define CS_MUX (ADC_MUX2)

#define IO_BITS_ALL (BIT(DRIVER_IO_BIT_IN1) | BIT(DRIVER_IO_BIT_IN2))

static uint16_t g_pwm_duty;
static uint32_t g_pwm_period;

static void init_state(void)
{
    g_pwm_duty = 0;
    g_pwm_period = PWM_DEFAULT_PERIOD;
}

void driver_init(void)
{
    pwm_init();

    pwm_disable();

    bit_set(DRIVER_IO_DDR, IO_BITS_ALL);

    bit_clear(DRIVER_IO_PORT, IO_BITS_ALL);

    adc_init_pins(DRIVER_AIN_BIT_CS);

    init_state();
}

void driver_get_state(
        driver_s * const state)
{
    state->cs = adc_read(CS_MUX);
    time_delay_ms(ADC_DEFAULT_SAMPLE_TIME_MS);
    state->cs = adc_read(CS_MUX);

    pwm_get_internal(
            &state->pwm_cs_bits,
            &state->pwm_ocr,
            &state->pwm_icr);

    state->in1 = (DRIVER_IO_PORT & BIT(DRIVER_IO_BIT_IN1)) >> DRIVER_IO_BIT_IN1;
    state->in2 = (DRIVER_IO_PORT & BIT(DRIVER_IO_BIT_IN2)) >> DRIVER_IO_BIT_IN2;

    state->en = g_pwm_duty;
    state->pwm_duty = g_pwm_duty;
    state->pwm_period = g_pwm_period;
}

void driver_set_direction(
        const uint8_t in1,
        const uint8_t in2)
{
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

void driver_toggle_direction(void)
{
    driver_set_direction(
            !(DRIVER_IO_PORT & BIT(DRIVER_IO_BIT_IN1)),
            !(DRIVER_IO_PORT & BIT(DRIVER_IO_BIT_IN2)));
}

void driver_enable(
        const uint16_t duty,
        const uint32_t period)
{
    g_pwm_duty = duty;
    g_pwm_period = period;

    pwm_enable(duty, period);
}

void driver_disable(void)
{
    pwm_disable();
    g_pwm_duty = 0;
}
