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

/**
 * It uses a binary search to find the integer square root of a 16-bit number
 *
 * By Triffid Hunter, Traumflug, jakepoz, many others.
 * https://github.com/Traumflug/Teacup_Firmware
 *
 * @param a The number to take the square root of.
 *
 * @return The square root of the input.
 */
uint16_t int_sqrt(uint32_t a)
{
    uint16_t b = a >> 16;
    uint8_t c = b >> 8;
    uint16_t x = 0;
    uint8_t z = 0;
    uint16_t i;
    uint8_t j;

    for (j = 0x8; j; j >>= 1)
    {
        uint8_t y2;

        z |= j;
        y2 = z * z;
        if (y2 > c)
            z ^= j;
    }

    x = z << 4;
    for (i = 0x8; i; i >>= 1)
    {
        uint16_t y2;

        x |= i;
        y2 = x * x;
        if (y2 > b)
            x ^= i;
    }

    x <<= 8;
    for (i = 0x80; i; i >>= 1)
    {
        uint32_t y2;

        x |= i;
        y2 = (uint32_t)x * x;
        if (y2 > a)
            x ^= i;
    }

    return x;
}

/**
 * Convert a number from one range to another
 *
 * @param num The number you want to convert
 * @param inMin The minimum value of the input range.
 * @param inMax The maximum value of the input range.
 * @param outMin The minimum value of the output range.
 * @param outMax The maximum value of the output range.
 *
 * @return The value of the number after it has been converted to the new range.
 */
float_t num_to_range(float num, float inMin, float inMax, float outMin, float outMax)
{
    return outMin + (float(num - inMin) / float(inMax - inMin) * (outMax - outMin));
}
