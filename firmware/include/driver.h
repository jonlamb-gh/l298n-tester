/**
 * @file driver.h
 * @brief TODO.
 *
 */

#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>

typedef struct
{
    uint8_t in1;
    uint8_t in2;
    uint16_t en;
    uint16_t cs;
    uint16_t pwm_duty;
    uint32_t pwm_period;
    uint8_t pwm_cs_bits;
    uint16_t pwm_ocr;
    uint16_t pwm_icr;
} driver_s;

void driver_init(void);

void driver_get_state(
        driver_s * const state);

void driver_set_direction(
        const uint8_t in1,
        const uint8_t in2);

void driver_toggle_direction(void);

void driver_enable(
        const uint16_t duty,
        const uint32_t period);

void driver_disable(void);

#endif  /* DRIVER_H */
