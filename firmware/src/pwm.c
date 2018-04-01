/**
 * @file pwm.c
 * @brief TODO.
 *
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "board_def.h"
#include "pwm.h"

#define TIMER3_RESOLUTION (65536UL)

// we're using pin A -> C4
#define COM3 (COM3A1)
#define OCR3 (OCR3A)

static uint8_t clock_select_bits;
static uint16_t pwm_period;

static void hw_init(void)
{
    bit_set(PWM_DDR, BIT(PWM_BIT));
    bit_clear(PWM_PORT, BIT(PWM_BIT));
}

static void resume(void)
{
    TCCR3B = BIT(WGM33) | clock_select_bits;
}

/*
static void start(void)
{
    TCCR3B = 0;
    TCNT3 = 0;

    resume();
}
*/

static void stop(void)
{
    TCCR3B = BIT(WGM33);
}

static void set_period(
        const uint32_t period)
{
    const uint32_t cycles = (F_CPU / 2000000UL) * period;

	if(cycles < TIMER3_RESOLUTION)
    {
	    clock_select_bits = BIT(CS30);
		pwm_period = cycles;
	}
    else if(cycles < TIMER3_RESOLUTION * 8)
    {
		clock_select_bits = BIT(CS31);
		pwm_period = cycles / 8;
	}
    else if(cycles < TIMER3_RESOLUTION * 64)
    {
		clock_select_bits = BIT(CS31) | BIT(CS30);
		pwm_period = cycles / 64;
	}
    else if(cycles < TIMER3_RESOLUTION * 256)
    {
		clock_select_bits = BIT(CS32);
		pwm_period = cycles / 256;
	}
    else if(cycles < TIMER3_RESOLUTION * 1024)
    {
		clock_select_bits = BIT(CS32) | BIT(CS30);
		pwm_period = cycles / 1024;
	}
    else
    {
		clock_select_bits = BIT(CS32) | BIT(CS30);
		pwm_period = TIMER3_RESOLUTION - 1;
	}

	ICR3 = pwm_period;
}

void pwm_init(void)
{
    hw_init();

    stop();

    // clear control register A
    TCCR3A = 0;

    clock_select_bits = 0;
    pwm_period = 0;

    set_period(PWM_DEFAULT_PERIOD);
}

void pwm_set_period(
        const uint32_t period)
{
    if(period > 0)
    {
        set_period(period);
    }

    // TODO - figure out the use case
    //resume();
}

void pwm_enable(
        const uint16_t duty)
{
    TCCR3A |= BIT(COM3);

    // set duty
    uint16_t duty_cycle = pwm_period;

    duty_cycle *= duty;
    duty_cycle >>= 10;

    OCR3 = duty_cycle;

    resume();
}

void pwm_disable(void)
{
    TCCR3A &= ~BIT(COM3);
    stop();
}
