/**
 * @file adc.c
 * @brief TODO.
 *
 * See: https://www.pjrc.com/teensy/adc.html
 *
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "board_def.h"
#include "adc.h"

#define MUX_MASK (0x1F)

#define ADC_REF_POWER BIT(REFS0)
#define ADC_REF_INTERNAL (BIT(REFS1) | BIT(REFS0))
#define ADC_REF_EXTERNAL (0)

#define DEFAULT_REF (ADC_REF_POWER)

// these prescaler values are for high speed mode, ADHSM = 1
#if F_CPU == 16000000L
#define ADC_PRESCALER (BIT(ADPS2) | BIT(ADPS1))
#elif F_CPU == 8000000L
#define ADC_PRESCALER (BIT(ADPS2) | BIT(ADPS0))
#elif F_CPU == 4000000L
#define ADC_PRESCALER (BIT(ADPS2))
#elif F_CPU == 2000000L
#define ADC_PRESCALER (BIT(ADPS1) | BIT(ADPS0))
#elif F_CPU == 1000000L
#define ADC_PRESCALER (BIT(ADPS1))
#else
#define ADC_PRESCALER (BIT(ADPS0))
#endif

void adc_init_pins(
        const uint8_t pin_bits)
{
    bit_clear(ADC_DDR, pin_bits);

    bit_set(DIDR0, pin_bits);
}

uint16_t adc_read(
        const uint8_t mux)
{
    uint8_t low;

    // enable ADC
	ADCSRA = BIT(ADEN) | ADC_PRESCALER;

    // high speed mode
	ADCSRB = BIT(ADHSM);

    // configure mux input
	ADMUX = DEFAULT_REF | (mux & MUX_MASK);

    // start conversion
	ADCSRA = BIT(ADEN) | ADC_PRESCALER | BIT(ADSC);

    // wait for completion
	while((ADCSRA & BIT(ADSC)) != 0);

    // read LSB first
	low = ADCL;

    // must only read MSB once
	return (uint16_t) ((ADCH << 8) | low);
}
