#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum {
    //                    ....xxxx....xxxx....xxxx....xxxx
    FMASK_SIGN = 0b10000000000000000000000000000000,
    FMASK_EXPN = 0b01111111100000000000000000000000,
    FMASK_FRAC = 0b00000000011111111111111111111111,
    FMASK_QNAN = 0b00000000010000000000000000000000,
    //                    ....xxxx....xxxx....xxxx....xxxx
    FFLAG_MASK = 0b00000000000000000000000000011111,
    FFLAG_INVALID_OP = 0b00000000000000000000000000010000,
    FFLAG_DIV_BY_ZERO = 0b00000000000000000000000000001000,
    FFLAG_OVERFLOW = 0b00000000000000000000000000000100,
    FFLAG_UNDERFLOW = 0b00000000000000000000000000000010,
    FFLAG_INEXACT = 0b00000000000000000000000000000001,
    //                    ....xxxx....xxxx....xxxx....xxxx
    RV_NAN = 0b01111111110000000000000000000000
};

static inline uint32_t calc_fclass(uint32_t f)
{
    const uint32_t sign = f & FMASK_SIGN;
    const uint32_t expn = f & FMASK_EXPN;
    const uint32_t frac = f & FMASK_FRAC;

    /* TODO: optimize with a binary decision tree */

    uint32_t out = 0;
    /* 0x001    rs1 is -INF */
    out |= (f == 0xff800000) ? 0x001 : 0;
    /* 0x002    rs1 is negative normal */
    out |= (expn && (expn != FMASK_EXPN) && sign) ? 0x002 : 0;
    /* 0x004    rs1 is negative subnormal */
    out |= (!expn && frac && sign) ? 0x004 : 0;
    /* 0x008    rs1 is -0 */
    out |= (f == 0x80000000) ? 0x008 : 0;
    /* 0x010    rs1 is +0 */
    out |= (f == 0x00000000) ? 0x010 : 0;
    /* 0x020    rs1 is positive subnormal */
    out |= (!expn && frac && !sign) ? 0x020 : 0;
    /* 0x040    rs1 is positive normal */
    out |= (expn && (expn != FMASK_EXPN) && !sign) ? 0x040 : 0;
    /* 0x080    rs1 is +INF */
    out |= (expn == FMASK_EXPN && !frac && !sign) ? 0x080 : 0;
    /* 0x100    rs1 is a signaling NaN */
    out |= (expn == FMASK_EXPN && frac && !(frac & FMASK_QNAN)) ? 0x100 : 0;
    /* 0x200    rs1 is a quiet NaN */
    out |= (expn == FMASK_EXPN && (frac & FMASK_QNAN)) ? 0x200 : 0;

    return out;
}

static inline uint32_t calc_fclass_new(uint32_t f)
{
    const uint32_t sign = f & FMASK_SIGN;
    const uint32_t expn = f & FMASK_EXPN;
    const uint32_t frac = f & FMASK_FRAC;

    uint32_t out = 0;

    /* 0x001    rs1 is -INF */
    /* 0x002    rs1 is negative normal */
    /* 0x004    rs1 is negative subnormal */
    /* 0x008    rs1 is -0 */
    /* 0x010    rs1 is +0 */
    /* 0x020    rs1 is positive subnormal */
    /* 0x040    rs1 is positive normal */
    /* 0x080    rs1 is +INF */
    /* 0x100    rs1 is a signaling NaN */
    /* 0x200    rs1 is a quiet NaN */

    // Check if it is special value -INF
    out |= (f == 0xff800000) ? 0x001 : 0;
    // Check if it is special value -0
    out |= (f == 0x80000000) ? 0x008 : 0;
    // Check if it is special value +0
    out |= (f == 0x00000000) ? 0x010 : 0;

    // Check the exponent bits
    if (expn) {
        if (expn != FMASK_EXPN) {
            // Check if it is negative normal or positive normal
            out |= sign ? 0x002 : 0x040;
        } else {
            // Check if it is NaN
            if (frac) {
                out |= frac & FMASK_QNAN ? 0x200 : 0x100;
            } else if (!sign) {
                // Check if it is +INF
                out |= 0x080;
            }
        }
    } else if (frac) {
        // Check if it is negative or positive subnormal
        out |= sign ? 0x004 : 0x020;
    }

    return out;
}

int main(int argc, char *argv[])
{
    uint32_t count1 = 0, count2 = 0;  // to prevent compiler from optimizing
    struct timespec t_start_origin;
    struct timespec t_start_new;
    struct timespec t_end_origin;
    struct timespec t_end_new;
    long long duration_origin;
    long long duration_new;
    int seed = (argc > 1 ? atoi(argv[1]) : time(NULL));

    t_start_origin.tv_sec = 0;
    t_start_origin.tv_nsec = 0;
    t_end_origin.tv_sec = 0;
    t_end_origin.tv_nsec = 0;
    t_start_new.tv_sec = 0;
    t_start_new.tv_nsec = 0;
    t_end_new.tv_sec = 0;
    t_end_new.tv_nsec = 0;

    srand(seed);
    clock_gettime(CLOCK_MONOTONIC, &t_start_origin);
    for (uint32_t i = 0; i < 0xffffffff; i++) {
        count1 |= calc_fclass(i);
    }
    clock_gettime(CLOCK_MONOTONIC, &t_end_origin);
    duration_origin = (1e9 * t_end_origin.tv_sec + t_end_origin.tv_nsec) -
                      (1e9 * t_start_origin.tv_sec + t_start_origin.tv_nsec);

    srand(seed);
    clock_gettime(CLOCK_MONOTONIC, &t_start_new);
    for (uint32_t i = 0; i < 0xffffffff; i++) {
        count2 |= calc_fclass_new(i);
    }
    clock_gettime(CLOCK_MONOTONIC, &t_end_new);
    duration_new = (1e9 * t_end_new.tv_sec + t_end_new.tv_nsec) -
                   (1e9 * t_start_new.tv_sec + t_start_new.tv_nsec);

    printf("  Elapsed time(old):   %lld\n", duration_origin);
    printf("  Elapsed time(new):   %lld\n", duration_new);
    if (duration_origin > duration_new) {
        printf("  New is faster by:    %f%%\n",
               (duration_origin - duration_new) * 100.0 / duration_origin);
    } else {
        printf("  Old is faster by:    %f%%\n",
               (duration_new - duration_origin) * 100.0 / duration_origin);
    }
    if (count1 != count2) {
        fprintf(stderr, "  Count1:   %d\n", count1);
        fprintf(stderr, "  Count2:   %d\n", count2);
        return 1;
    }
    return 0;
}
