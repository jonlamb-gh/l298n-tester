/**
 * @file input.h
 * @brief TODO.
 *
 */

#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#define INPUT_DEBOUNCE_TIME_MS (20UL)

typedef struct
{
    uint8_t bt0;
    uint8_t bt1;
    uint8_t bt2;
    uint8_t sw0;
    uint16_t pt0;
    uint16_t pt1;
} input_s;

void input_init(void);

void input_update(
        input_s * const input);

#endif  /* INPUT_H */
