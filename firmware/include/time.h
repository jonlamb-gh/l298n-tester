/**
 * @file time.h
 * @brief TODO.
 *
 */

#ifndef TIME_H
#define TIME_H

#include <stdint.h>

void time_init(void);

void time_delay_ms(
        const uint16_t ms);

uint32_t time_get_ms(void);

#endif  /* TIME_H */
