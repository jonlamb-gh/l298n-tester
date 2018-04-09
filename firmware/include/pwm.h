/**
 * @file pwm.h
 * @brief TODO.
 *
 */

#ifndef PWM_H
#define PWM_H

#include <stdint.h>

#define PWM_DEFAULT_PERIOD (1000000UL)

//#define PWM_PERIOD_MAX (4194240UL)
// 1/8
#define PWM_PERIOD_MAX (1048560UL)
#define PWM_DUTY_MAX (1023UL)

void pwm_init(void);

void pwm_set_period(
        const uint32_t period);

void pwm_enable(
        const uint16_t duty);

void pwm_disable(void);

#endif  /* PWM_H */
