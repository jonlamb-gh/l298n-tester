/**
 * @file input.h
 * @brief TODO.
 *
 */

#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#define INPUT_DEBOUNCE_TIME_MS (20UL)

#define INPUT_ALL (0xFF)
#define INPUT_NONE (0x00)
// btn0:2
#define INPUT_GROUP_BUTTON (0x07)
// sw0
#define INPUT_GROUP_SWITCH (0x08)
// pot0:1
#define INPUT_GROUP_POT (0x30)

typedef struct __attribute__((packed))
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
        const uint8_t flags,
        input_s * const input);

#endif /* INPUT_H */
