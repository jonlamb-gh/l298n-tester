/**
 * @file time.h
 * @brief TODO.
 *
 */

#ifndef TIME_H
#define TIME_H

#include <stdint.h>

#define US_PER_MS (1000UL)

#define TIME_EVENT_PERIOD_MS (250UL)

void time_init(void);

void time_delay_ms(
        const uint16_t ms);

uint32_t time_get_ms(void);

uint8_t time_get_and_clear_event(void);

#endif /* TIME_H */
