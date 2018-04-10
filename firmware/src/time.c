/**
 * @file time.c
 * @brief TODO.
 *
 * Use this where needed: https://www.nongnu.org/avr-libc/user-manual/group__util__delay.html
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "board_def.h"
#include "time.h"

#define timer8_clear() {TCCR0A=0; TCNT0=0;}
#define timer8_set_counter(value) (TCNT0 = value)
#define timer8_set_compare_a(value) (OCR0A = value)
#define timer8_overflow_it_enable() (TIMSK0 |= BIT(TOIE0))
#define timer8_overflow_it_disable() (TIMSK0 &= ~BIT(TOIE0))
#define timer8_compare_a_it_enable() (TIMSK0 |= BIT(OCIE0A))
#define timer8_compare_a_it_disable() ( TIMSK0 &= ~BIT(OCIE0A))
#define timer8_clear_overflow_it() (TIFR0 |= BIT(TOV0))
#define timer8_clear_compare_a_it() (TIFR0 |= BIT(OCF0A))

static volatile uint32_t g_counter_ms;
static volatile uint32_t g_timer_counter;
static volatile uint8_t g_timer_reached;

ISR(TIMER0_COMPA_vect)
{
    g_counter_ms += 1;
    g_timer_counter += 1;

    if(g_timer_counter == TIME_EVENT_PERIOD_MS)
    {
        g_timer_counter = 0;
        g_timer_reached = 1;
    }
}

static void delay_ms(
        uint16_t ms)
{
    while(ms != 0)
    {
        _delay_ms(1);
        ms -= 1;
    }
}

void time_init(void)
{
    disable_interrupt();

    timer8_clear();

    // wait for Xtal to stabilize
    uint16_t i;
    for(i = 0; i < 0xFFFF; i += 1);

    timer8_overflow_it_disable();
    timer8_compare_a_it_disable();

    // 1 KHz counter on timer0
    TCCR0A |= BIT(WGM01);
    TCCR0B |= BIT(CS01) | BIT(CS00);
    timer8_set_counter(0);
    timer8_set_compare_a(0xFA);

    timer8_clear_overflow_it();
    timer8_clear_compare_a_it();
    timer8_compare_a_it_enable();

    for(i = 0; i < 0xFFFF; i += 1);

    g_counter_ms = 0;
    g_timer_counter = 0;
    g_timer_reached = 0;

    enable_interrupt();
}

void time_delay_ms(
        const uint16_t ms)
{
    if(ms != 0)
    {
        delay_ms(ms);
    }
}

uint32_t time_get_ms(void)
{
    disable_interrupt();
    const uint32_t timestamp = g_counter_ms;
    enable_interrupt();

    return timestamp;
}

uint8_t time_get_and_clear_event(void)
{
    disable_interrupt();

    const uint8_t status = g_timer_reached;

    g_timer_reached = 0;

    enable_interrupt();

    return status;
}
