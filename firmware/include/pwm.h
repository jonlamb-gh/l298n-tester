/**
 * @file pwm.h
 * @brief TODO.
 *
 */

#ifndef PWM_H
#define PWM_H

#include <stdint.h>

#define PWM_DEFAULT_PERIOD (1000000UL)

#define PWM_PERIOD_MAX (4194240UL)
#define PWM_DUTY_MAX (1023UL)

typedef void (*pwm_interrupt_callback)(
        const uint8_t top);

void pwm_init(void);

void pwm_enable(
        const uint16_t duty,
        const uint32_t period);

void pwm_disable(void);

void pwm_set_interrupt_callback(
        const pwm_interrupt_callback cb);

void pwm_get_internal(
        uint8_t * const cs_bits,
        uint16_t * const ocr,
        uint16_t * const icr);

#endif /* PWM_H */
