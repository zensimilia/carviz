#include "utils.h"

/**
 * Find integer square root on number (recursive)
 *
 * @param n The number to find the square root of.
 *
 * @return The integer square root of the input.
 */
uint32_t isqrt(uint32_t n)
{
    uint32_t small, large;

    if (n < 2)
        return n;

    small = isqrt(n >> 2) << 1;
    large = small + 1;

    return (large * large > n) ? small : large;
}
