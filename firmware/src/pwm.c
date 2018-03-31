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

static uint8_t clock_select_bits;

static void resume(void)
{
    TCCR3B = BIT(WGM33) | clock_select_bits;
}

static void start(void)
{
    TCCR3B = 0;
    TCNT3 = 0;

    resume();
}

static void stop(void)
{
    TCCR3B = BIT(WGM33);
}

static void set_period(
        const uint32_t period)
{
    uint16_t pwm_period = 0;
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
	TCCR3B = BIT(WGM33) | clock_select_bits;
}

void pwm_init(void)
{
    stop();

    // clear control register A
    TCCR3A = 0;
    clock_select_bits = 0;

    set_period(PWM_DEFAULT_PERIOD);
}

void pwm_set_period(
        const uint32_t period)
{
    if(period > 0)
    {
        set_period(period);
    }
}

void pwm_enable(
        const uint16_t duty)
{

}

void pwm_disable(void)
{

}
