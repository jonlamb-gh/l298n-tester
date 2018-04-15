/**
 * @file pwm.c
 * @brief TODO.
 *
 * See:
 * - http://playground.arduino.cc/Code/Timer1
 * - https://github.com/PaulStoffregen/TimerThree/blob/master/TimerThree.h
 *
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "board_def.h"
#include "pwm.h"

#define TIMER3_RESOLUTION (65536UL)

// pin C4 maps to channel C
#define COM3 (COM3C1)
#define OCR3 (OCR3C)

static uint8_t g_cs_bits;
static uint16_t g_pwm_period;
static pwm_interrupt_callback g_interrupt_cb;

ISR(TIMER3_CAPT_vect)
{
    if(g_interrupt_cb != NULL)
    {
        // top
        g_interrupt_cb(1);
    }
}

ISR(TIMER3_OVF_vect)
{
    if(g_interrupt_cb != NULL)
    {
        // bottom
        g_interrupt_cb(0);
    }
}

static void hw_init(void)
{
    bit_set(PWM_DDR, BIT(PWM_BIT));
    bit_clear(PWM_PORT, BIT(PWM_BIT));
}

static void resume(void)
{
    TCCR3B = BIT(WGM33) | g_cs_bits;
}

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
	    g_cs_bits = BIT(CS30);
		g_pwm_period = cycles;
	}
    else if(cycles < TIMER3_RESOLUTION * 8)
    {
		g_cs_bits = BIT(CS31);
		g_pwm_period = cycles / 8;
	}
    else if(cycles < TIMER3_RESOLUTION * 64)
    {
		g_cs_bits = BIT(CS31) | BIT(CS30);
		g_pwm_period = cycles / 64;
	}
    else if(cycles < TIMER3_RESOLUTION * 256)
    {
		g_cs_bits = BIT(CS32);
		g_pwm_period = cycles / 256;
	}
    else if(cycles < TIMER3_RESOLUTION * 1024)
    {
		g_cs_bits = BIT(CS32) | BIT(CS30);
		g_pwm_period = cycles / 1024;
	}
    else
    {
		g_cs_bits = BIT(CS32) | BIT(CS30);
		g_pwm_period = TIMER3_RESOLUTION - 1;
	}

	ICR3 = g_pwm_period;
}

void pwm_init(void)
{
    hw_init();

    stop();

    TCCR3A = 0;
    TIMSK3 &= ~(BIT(TOIE3) | BIT(ICIE3));

    g_cs_bits = 0;
    g_pwm_period = 0;
    g_interrupt_cb = NULL;

    set_period(PWM_DEFAULT_PERIOD);
}

void pwm_enable(
        const uint16_t duty,
        const uint32_t period)
{
    stop();

    if(period > 0)
    {
        set_period(period);
    }

    TCCR3A |= BIT(COM3);

    uint16_t duty_cycle = g_pwm_period;

    duty_cycle *= duty;
    duty_cycle >>= 10;

    OCR3 = duty_cycle;

    resume();
}

void pwm_disable(void)
{
    TCCR3A &= ~BIT(COM3);
    stop();
    TIMSK3 &= ~(BIT(TOIE3) | BIT(ICIE3));
    bit_clear(PWM_PORT, BIT(PWM_BIT));
}

void pwm_set_interrupt_callback(
        const pwm_interrupt_callback cb)
{
    if(cb == NULL)
    {
        TIMSK3 &= ~(BIT(TOIE3) | BIT(ICIE3));

        disable_interrupt();
        g_interrupt_cb = NULL;
        enable_interrupt();
    }
    else
    {
        TIMSK3 &= ~(BIT(TOIE3) | BIT(ICIE3));

        disable_interrupt();
        g_interrupt_cb = cb;
        enable_interrupt();

        TIMSK3 |= (BIT(TOIE3) | BIT(ICIE3));
    }
}

void pwm_get_internal(
        uint8_t * const cs_bits,
        uint16_t * const ocr,
        uint16_t * const icr)
{
    *cs_bits = g_cs_bits;
    *ocr = OCR3;
    *icr = ICR3;
}
