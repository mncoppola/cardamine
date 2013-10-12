void init_cpu(void);
char read_byte(void);
void exec_instruction(void);

/* CPU run state */
char halt;
unsigned int cpu_cycles;
unsigned int total_cpu_cycles;

/* General 8-bit data registers */
unsigned char a, b, c, d, e, h, l, flags;

/* Flow control registers */
unsigned short pc, sp;

/* Convenience for 16-bit address registers */
#define GET_PAIR(o, t) ((o << 8) | t)
#define GET_AF()       GET_PAIR(a, flags)
#define GET_BC()       GET_PAIR(b, c)
#define GET_DE()       GET_PAIR(d, e)
#define GET_HL()       GET_PAIR(h, l)
#define GET_SP()       sp
#define GET_PC()       pc

#define SET_PAIR(o, t, v) do { o = (v) >> 8; t = (v) & 0xff; } while (0);
#define SET_AF(val)       SET_PAIR(a, flags, val)
#define SET_BC(val)       SET_PAIR(b, c, val)
#define SET_DE(val)       SET_PAIR(d, e, val)
#define SET_HL(val)       SET_PAIR(h, l, val)
#define SET_SP(val)       sp = val
#define SET_PC(val)       pc = val

#define ROL(val) ((val << 1) | (val >> (sizeof(val) - 1)))
#define ROR(val) ((val >> 1) | (val << (sizeof(val) - 1)))

/* Flag bitmasks and respective right shifts for parsing */
#define Z_FLAG  0x80
#define N_FLAG  0x40
#define H_FLAG  0x20
#define C_FLAG  0x10
#define Z_SHIFT 7
#define N_SHIFT 6
#define H_SHIFT 5
#define C_SHIFT 4

/* Bit twiddling utils for flags with side-effects */
#define TEST_FLAG(flag)  ((flags & flag##_FLAG) >> flag##_SHIFT)
#define SET_FLAG(flag)   (flags |= flag##_FLAG)
#define CLEAR_FLAG(flag) (flags &= ~flag##_FLAG)
#define FLIP_FLAG(flag)  (flags ^= flag##_FLAG)
#define COND_FLAG(flag, cond) do { \
    if ( cond )                         \
        SET_FLAG(flag);                 \
    else                                \
        CLEAR_FLAG(flag);               \
} while(0)

/* Carry and half-carry detection */
#define CARRY(orig, result)                ((unsigned)orig > (unsigned)result)
#define BORROW(orig, result)               ((unsigned)result > (unsigned)orig)
#define HALFCARRY8(orig, added, result)    0 //(((orig ^ added ^ result) & 0x10) >> 4)
#define HALFCARRY16(orig, added, result)   0
#define HALFBORROW8(orig, subbed, result)  0
#define HALFBORROW16(orig, subbed, result) 0

/* Abstractions for common instructions and operations */
#define PUSH(value) do { sp -= 2; set_mem16(sp, (value)); } while (0)
#define POP(reg16)  do { SET_##reg16(get_mem16(sp)); sp += 2; } while (0)
#define CALL(addr)  do { PUSH(pc); pc = (addr); } while (0)
#define RET()       do { pc = get_mem16(sp); sp += 2; } while (0)

#define ADD(to, add) do {                      \
    char orig = to;                            \
    char toadd = add;                          \
    to += toadd;                               \
    COND_FLAG(Z, (to == 0));                   \
    CLEAR_FLAG(N);                             \
    COND_FLAG(H, HALFCARRY8(orig, toadd, to)); \
    COND_FLAG(C, CARRY(orig, to));             \
} while (0)

#define ADD16(to, add) do {                             \
    unsigned short orig = GET_##to();                   \
    unsigned short toadd = GET_##add();                 \
    SET_##to(orig + toadd);                             \
    CLEAR_FLAG(N);                                      \
    COND_FLAG(H, HALFCARRY16(orig, toadd, GET_##to())); \
    COND_FLAG(C, CARRY(orig, GET_##to()));              \
} while (0)

#define ADC(to, add) do {                      \
    char orig = to;                            \
    char toadd = add + TEST_FLAG(C);           \
    to += toadd;                               \
    COND_FLAG(Z, (to == 0));                   \
    CLEAR_FLAG(N);                             \
    COND_FLAG(H, HALFCARRY8(orig, toadd, to)); \
    COND_FLAG(C, CARRY(orig, to));             \
} while (0)

#define SUB(to, sub) do {                        \
    char orig = to;                              \
    char tosub = sub;                            \
    to -= tosub;                                 \
    COND_FLAG(Z, (to == 0));                     \
    SET_FLAG(N);                                 \
    COND_FLAG(H, !HALFBORROW8(orig, tosub, to)); \
    COND_FLAG(C, !BORROW(orig, to));             \
} while (0)

#define SBC(to, sub) do {                        \
    char orig = to;                              \
    char tosub = sub + TEST_FLAG(C);             \
    to -= tosub;                                 \
    COND_FLAG(Z, (to == 0));                     \
    SET_FLAG(N);                                 \
    COND_FLAG(H, !HALFBORROW8(orig, tosub, to)); \
    COND_FLAG(C, !BORROW(orig, to));             \
} while (0)

#define BITWISE(to, from, op, hf) do { \
    (to) op (from);                    \
    COND_FLAG(Z, ((to) == 0));         \
    CLEAR_FLAG(N);                     \
    COND_FLAG(H, hf);                  \
    CLEAR_FLAG(C);                     \
} while (0)

#define COMP(to, from) do {                       \
    char result = to - from;                      \
    COND_FLAG(Z, (result == 0));                  \
    SET_FLAG(N);                                  \
    COND_FLAG(H, !HALFBORROW8(to, from, result)); \
    COND_FLAG(C, !BORROW(to, result));            \
} while (0)

#define INC(to) do {                           \
    to++;                                      \
    COND_FLAG(Z, (to == 0));                   \
    CLEAR_FLAG(N);                             \
    COND_FLAG(H, HALFCARRY8((to - 1), 1, to)); \
} while (0)

#define RLC(to) do {           \
    COND_FLAG(C, (to & 0x80)); \
    to = ROL(to);              \
    COND_FLAG(Z, (to == 0));   \
    CLEAR_FLAG(N);             \
    CLEAR_FLAG(H);             \
} while (0)

#define RL(to) do {            \
    char tmp = TEST_FLAG(C);   \
    COND_FLAG(C, (to & 0x80)); \
    to = ((to << 1) | tmp);    \
    COND_FLAG(Z, (to == 0));   \
    CLEAR_FLAG(N);             \
    CLEAR_FLAG(H);             \
} while (0)

#define SLA(to) do {           \
    COND_FLAG(C, (to & 0x80)); \
    to <<= 1;                  \
    COND_FLAG(Z, (to == 0));   \
    CLEAR_FLAG(N);             \
    CLEAR_FLAG(H);             \
} while (0)

#define RRC(to) do {           \
    COND_FLAG(C, (to & 0x1));  \
    to = ROR(to);              \
    COND_FLAG(Z, (to == 0));   \
    CLEAR_FLAG(N);             \
    CLEAR_FLAG(H);             \
} while (0)

#define RR(to) do {                \
    char tmp = TEST_FLAG(C);       \
    COND_FLAG(C, (to & 0x1));      \
    to = ((to >> 1) | (tmp << 7)); \
    COND_FLAG(Z, (to == 0));       \
    CLEAR_FLAG(N);                 \
    CLEAR_FLAG(H);                 \
} while (0)

#define SRA(to) do {              \
    COND_FLAG(C, (to & 0x1));     \
    to = (to >> 1) | (to & 0x80); \
    COND_FLAG(Z, (to == 0));      \
    CLEAR_FLAG(N);                \
    CLEAR_FLAG(H);                \
} while (0)

#define SRL(to) do {              \
    COND_FLAG(C, (to & 0x1));     \
    to >>= 1;                     \
    COND_FLAG(Z, (to == 0));      \
    CLEAR_FLAG(N);                \
    CLEAR_FLAG(H);                \
} while (0)

#define INC16(to) SET_##to(GET_##to() + 1)
#define DEC16(to) SET_##to(GET_##to() - 1)

#define DEC(to) do {                             \
    to--;                                        \
    COND_FLAG(Z, (to == 0));                     \
    SET_FLAG(N);                                 \
    COND_FLAG(H, !HALFBORROW8((to + 1), 1, to)); \
} while (0)

#define SWAP(reg8) do {                                \
    reg8 = ((reg8 & 0xf) << 4) | ((reg8 & 0xf0) >> 4); \
    COND_FLAG(Z, (reg8 == 0));                         \
    CLEAR_FLAG(N);                                     \
    CLEAR_FLAG(H);                                     \
    CLEAR_FLAG(C);                                     \
} while (0)

#define BIT(reg8, b) do {             \
    COND_FLAG(Z, !TEST_BIT(reg8, b)); \
    CLEAR_FLAG(N);                    \
    SET_FLAG(H);                      \
} while (0)
