#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Bit twiddling utils for registers without side-effects */
#define TEST_BIT(reg8, bit)  ((reg8 & (1 << bit)) >> bit)
#define SET_BIT(reg8, bit)   (reg8 | (1 << bit))
#define CLEAR_BIT(reg8, bit) (reg8 & ~(1 << bit))
#define FLIP_BIT(reg8, bit)  (reg8 ^ (1 << bit))

#define PARITY(reg8) ((0x6996 >> ((reg8 ^ (reg8 >> 4)) & 0xf)) & 1)
#define BITFIELD(a, b, c, d, e, f, g, h) (((a) & 1)       | \
                                         (((b) & 1) << 1) | \
                                         (((c) & 1) << 2) | \
                                         (((d) & 1) << 3) | \
                                         (((e) & 1) << 4) | \
                                         (((f) & 1) << 5) | \
                                         (((g) & 1) << 6) | \
                                         (((h) & 1) << 7))
