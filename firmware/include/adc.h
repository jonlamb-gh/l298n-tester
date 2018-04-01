/**
 * @file adc.h
 * @brief TODO.
 *
 */

#ifndef ADC_H
#define ADC_H

#include <stdint.h>

// MUX for ADC0:7
#define ADC_MUX0 (0x00)
#define ADC_MUX1 (0x01)
#define ADC_MUX2 (0x02)

void adc_init_pins(
        const uint8_t pin_bits);

uint16_t adc_read(
        const uint8_t mux);

#endif  /* ADC_H */