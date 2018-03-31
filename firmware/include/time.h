/**
 * @file time.h
 * @brief TODO.
 *
 */

#ifndef TIME_H
#define TIME_H

#include <stdint.h>

#define US_PER_MS (1000UL)

void time_init(void);

void time_delay_ms(
        const uint16_t ms);

void time_delay_us(
        const uint32_t * const us);

uint32_t time_get_us(void);

#endif  /* TIME_H */
