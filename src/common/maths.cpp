#include "maths.h"
#include <intrin.h>

uint32_t clz32(uint32_t val)
{
#if defined(__GNUC__)
    return val ? __builtin_clz(val) : 32;
#else
    /* Binary search for the leading one bit.  */
    int cnt = 0;

    if ((val & 0xFFFF0000U) == 0)
    {
        cnt += 16;
        val <<= 16;
    }
    if ((val & 0xFF000000U) == 0)
    {
        cnt += 8;
        val <<= 8;
    }
    if ((val & 0xF0000000U) == 0)
    {
        cnt += 4;
        val <<= 4;
    }
    if ((val & 0xC0000000U) == 0)
    {
        cnt += 2;
        val <<= 2;
    }
    if ((val & 0x80000000U) == 0)
    {
        cnt++;
        val <<= 1;
    }
    if ((val & 0x80000000U) == 0)
    {
        cnt++;
    }
    return cnt;
#endif
}

uint32_t clz64(uint64_t val)
{
#if defined(__GNUC__)
    return val ? __builtin_clzll(val) : 64;
#else
    int cnt = 0;

    if ((val >> 32) == 0)
    {
        cnt += 32;
    }
    else
    {
        val >>= 32;
    }

    return cnt + clz32((uint32_t)val);
#endif
}

uint64_t mull128_u64(uint64_t multiplier, uint64_t multiplicand, uint64_t * high)
{
#if defined(_MSC_VER)
    return _umul128(multiplier, multiplicand, high);
#else
    // multiplier   = ab = a * 2^32 + b
    // multiplicand = cd = c * 2^32 + d
    // ab * cd = a * c * 2^64 + (a * d + b * c) * 2^32 + b * d
    uint64_t a = multiplier >> 32;
    uint64_t b = (uint32_t)multiplier; // & 0xFFFFFFFF;
    uint64_t c = multiplicand >> 32;
    uint64_t d = (uint32_t)multiplicand; // & 0xFFFFFFFF;

    uint64_t ad = __emulu(a, d);
    uint64_t bd = __emulu(b, d);

    uint64_t adbc = ad + __emulu(b, c);
    uint64_t adbc_carry = (adbc < ad); // ? 1 : 0;

    // multiplier * multiplicand = product_hi * 2^64 + product_lo
    uint64_t product_lo = bd + (adbc << 32);
    uint64_t product_lo_carry = (product_lo < bd); // ? 1 : 0;
    *high = __emulu(a, c) + (adbc >> 32) + (adbc_carry << 32) + product_lo_carry;

    return product_lo;
#endif
}

uint64_t div128_to_64(uint64_t u1, uint64_t u0, uint64_t v, uint64_t * r)
{
    const uint64_t b = (1ULL << 32); // Number base (32 bits)
    uint64_t un1, un0;               // Norm. dividend LSD's
    uint64_t vn1, vn0;               // Norm. divisor digits
    uint64_t q1, q0;                 // Quotient digits
    uint64_t un64, un21, un10;       // Dividend digit pairs
    uint64_t rhat;                   // A remainder
    int32_t s;                       // Shift amount for norm

    // If overflow, set rem. to an impossible value,
    // and return the largest possible quotient
    if (u1 >= v)
    {
        *r = (uint64_t)-1;
        return (uint64_t)-1;
    }

    // count leading zeros
    s = clz64(v);
    if (s > 0)
    {
        // Normalize divisor
        v = v << s;
        un64 = (u1 << s) | (u0 >> (64 - s));
        un10 = u0 << s; // Shift dividend left
    }
    else
    {
        // Avoid undefined behavior of (u0 >> 64).
        // The behavior is undefined if the right operand is
        // negative, or greater than or equal to the length
        // in bits of the promoted left operand.
        un64 = u1;
        un10 = u0;
    }

    // Break divisor up into two 32-bit digits
    vn1 = v >> 32;
    vn0 = v & 0xFFFFFFFF;

    // Break right half of dividend into two digits
    un1 = un10 >> 32;
    un0 = un10 & 0xFFFFFFFF;

    // Compute the first quotient digit, q1
    q1 = un64 / vn1;
    rhat = un64 - q1 * vn1;

    while (q1 >= b || q1 * vn0 > b * rhat + un1)
    {
        q1 = q1 - 1;
        rhat = rhat + vn1;
        if (rhat >= b)
        {
            break;
        }
    }

    // Multiply and subtract
    un21 = un64 * b + un1 - q1 * v;

    // Compute the second quotient digit
    q0 = un21 / vn1;
    rhat = un21 - q0 * vn1;

    while (q0 >= b || q0 * vn0 > b * rhat + un0)
    {
        q0 = q0 - 1;
        rhat = rhat + vn1;
        if (rhat >= b)
        {
            break;
        }
    }

    *r = (un21 * b + un0 - q0 * v) >> s;
    return q1 * b + q0;
}
