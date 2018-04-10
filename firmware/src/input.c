/**
 * @file input.c
 * @brief TODO.
 *
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "board_def.h"
#include "time.h"
#include "adc.h"
#include "input.h"

#define BUTTON_BITS_ALL (BIT(BUTTON_BIT_BT0) | BIT(BUTTON_BIT_BT1) | BIT(BUTTON_BIT_BT2))
#define SWITCH_BITS_ALL (BIT(SWITCH_BIT_SW0))
#define POT_BITS_ALL (BIT(POT_BIT_PT0) | BIT(POT_BIT_PT1))

#define PT0_MUX (ADC_MUX0)
#define PT1_MUX (ADC_MUX1)

static void init_buttons(void)
{
    bit_clear(BUTTON_DDR, BUTTON_BITS_ALL);

    // enable pull-ups
    bit_set(BUTTON_PORT, BUTTON_BITS_ALL);
}

static void init_switches(void)
{
    bit_clear(SWITCH_DDR, SWITCH_BITS_ALL);

    // enable pull-ups
    bit_set(SWITCH_PORT, SWITCH_BITS_ALL);
}

static void init_potentiometers(void)
{
    adc_init_pins(POT_BITS_ALL);
}

static uint8_t read_buttons(void)
{
    uint8_t set = (BUTTON_PIN & BUTTON_BITS_ALL);

    // active-low
    if(set != BUTTON_BITS_ALL)
    {
        // one or more is active
        time_delay_ms(INPUT_DEBOUNCE_TIME_MS);

        // TODO - real debounce
        set = (BUTTON_PIN & BUTTON_BITS_ALL);
    }

    // return an active-high set
    set = ~set & BUTTON_BITS_ALL;

    return set;
}

static uint8_t read_switches(void)
{
    uint8_t set = (SWITCH_PIN & SWITCH_BITS_ALL);

    // active-low
    if(set != SWITCH_BITS_ALL)
    {
        // one or more is active
        time_delay_ms(INPUT_DEBOUNCE_TIME_MS);

        // TODO - real debounce
        set = (SWITCH_PIN & SWITCH_BITS_ALL);
    }

    // return an active-high set
    set = ~set & SWITCH_BITS_ALL;

    return set;
}

void input_init(void)
{
    init_buttons();
    init_switches();
    init_potentiometers();
}

void input_update(
        const uint8_t flags,
        input_s * const input)
{
    if((flags & INPUT_GROUP_BUTTON) != 0)
    {
        const uint8_t btn_set = read_buttons();

        input->bt0 = (btn_set >> BUTTON_BIT_BT0) & 0x01;
        input->bt1 = (btn_set >> BUTTON_BIT_BT1) & 0x01;
        input->bt2 = (btn_set >> BUTTON_BIT_BT2) & 0x01;
    }

    if((flags & INPUT_GROUP_SWITCH) != 0)
    {
        const uint8_t sw_set = read_switches();

        input->sw0 = (sw_set >> SWITCH_BIT_SW0) & 0x01;
    }

    if((flags & INPUT_GROUP_POT) != 0)
    {
        input->pt0 = adc_read(PT0_MUX);
        time_delay_ms(ADC_DEFAULT_SAMPLE_TIME_MS);
        input->pt0 = adc_read(PT0_MUX);

        input->pt1 = adc_read(PT1_MUX);
        time_delay_ms(ADC_DEFAULT_SAMPLE_TIME_MS);
        input->pt1 = adc_read(PT1_MUX);
    }
}
