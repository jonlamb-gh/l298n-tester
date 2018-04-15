/**
 * @file math.h
 * @brief TODO.
 *
 */

#ifndef MATH_H
#define MATH_H

#include <stdint.h>

static inline int32_t map_i32(
        const int32_t x,
        const int32_t in_min,
        const int32_t in_max,
        const int32_t out_min,
        const int32_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif /* MATH_H */
