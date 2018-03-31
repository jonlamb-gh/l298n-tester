/**
 * @file time.c
 * @brief TODO.
 *
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "board_def.h"
#include "time.h"

#define timer8_clear() {TCCR0A=0; TCNT0=0; OCR0A=0;}
#define timer8_set_counter(value) (TCNT0 = value)
#define timer8_set_compare_a(value) (OCR0A = value)
#define timer8_overflow_it_enable() (TIMSK0 |= BIT(TOIE0))
#define timer8_overflow_it_disable() (TIMSK0 &= ~BIT(TOIE0))
#define timer8_compare_a_it_enable() (TIMSK0 |= BIT(OCIE0A))
#define timer8_compare_a_it_disable() ( TIMSK0 &= ~BIT(OCIE0A))
#define timer8_clear_overflow_it() (TIFR0 |= BIT(TOV0))
#define timer8_clear_compare_a_it() (TIFR0 |= BIT(OCF0A))

static volatile uint32_t global_counter_us;

ISR(TIMER0_COMPA_vect)
{
    global_counter_us += 1;
}

static void delay_us(
        const uint32_t * const us)
{
    uint8_t done;
    uint32_t time;

    // TODO - make this better
    disable_interrupt();
    time = global_counter_us;
    enable_interrupt();

    time += *us;
    done = 0;

    while(done == 0)
    {
        disable_interrupt();

        if(global_counter_us >= time)
        {
            done = 1;
        }
        else if(global_counter_us == *us)
        {
            // bad overflow detection
            done = 1;
        }

        enable_interrupt();
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

    // 1 MHz counter on timer0
    TCCR0A |= BIT(WGM01);
    TCCR0B |= BIT(CS00);
    timer8_set_counter(0);
    timer8_set_compare_a(0x10);

    timer8_clear_overflow_it();
    timer8_clear_compare_a_it();
    timer8_compare_a_it_enable();

    for(i = 0; i < 0xFFFF; i += 1);

    global_counter_us = 0;

    enable_interrupt();
}

void time_delay_ms(
        const uint16_t ms)
{
    const uint32_t us = (uint32_t) ms * US_PER_MS;

    if(us != 0)
    {
        delay_us(&us);
    }
}

void time_delay_us(
        const uint32_t * const us)
{
    if(*us != 0)
    {
        delay_us(us);
    }
}

uint32_t time_get_us(void)
{
    disable_interrupt();
    const uint32_t timestamp = global_counter_us;
    enable_interrupt();

    return timestamp;
}
