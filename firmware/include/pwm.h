/**
 * @file pwm.h
 * @brief TODO.
 *
 */

#ifndef PWM_H
#define PWM_H

#include <stdint.h>

#define PWM_DEFAULT_PERIOD (1000000UL)

void pwm_init(void);

void pwm_set_period(
        const uint32_t period);

void pwm_enable(
        const uint16_t duty);

void pwm_disable(void);

#endif  /* PWM_H */
