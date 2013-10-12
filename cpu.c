#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "common.h"
#include "cpu.h"
#include "mem.h"

#define __DEBUG__ 1

#if __DEBUG__
#define DEBUG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define DEBUG(fmt, ...)
#endif

/* CPU run state */
char halt;
unsigned int cpu_cycles;
unsigned int total_cpu_cycles;

/* General 8-bit data registers */
unsigned char a, b, c, d, e, h, l, flags;

/* Flow control registers */
unsigned short pc, sp;

/* Interrupt master enable flag */
char ime;

/* Return a byte at pc */
char peek_byte ( void )
{
    return get_mem8(pc);
}

/* Return a byte at pc and advance pc by 1 */
char read_byte ( void )
{
    char byte = peek_byte();
//    printf("Reading byte %hhx\n", byte);
    pc++;
    return byte;
}

/* Return two bytes at pc */
short peek_word ( void )
{
    if ( (0x10000 - pc) < 0x2 )
    {
        printf("Error!  Referencing outside of address space.\n");
        return 0; // Look up how GameBoy handles this exception
    }

    return get_mem16(pc);
}

/* Return two bytes at pc and advance pc by 2 */
short read_word ( void )
{
    short word = peek_word();
//    printf("Reading word %hx\n", word);
    pc += 2;
    return word;
}

void dump_regs ( void )
{
    printf("pc=%04hx sp=%04hx flags=%02hhx\n", pc, sp, flags);
    printf("a=%02hhx b=%02hhx c=%02hhx d=%02hhx e=%02hhx h=%02hhx l=%02hhx\n\n", a, b, c, d, e, h, l);
}

void exec_instruction ( void )
{
    unsigned char op;

    DEBUG("%04hx  ", pc);

    op = read_byte();

    switch ( op )
    {
        /* NOP */
        case 0x00:
            DEBUG("nop\n");
            cpu_cycles = 4;
            break;

        /* LD BC, nn */
        case 0x01:
        {
            unsigned short tmp = read_word();
            DEBUG("ld bc, 0x%hx\n", tmp);
            SET_BC(tmp);
            cpu_cycles = 12;
            break;
        }

        /* LD (BC), A */
        case 0x02:
            DEBUG("ld (bc), a\n");
            set_mem8(GET_BC(), a);
            cpu_cycles = 8;
            break;

        /* INC BC */
        case 0x03:
            DEBUG("inc bc\n");
            INC16(BC);
            cpu_cycles = 8;
            break;

        /* INC B */
        case 0x04:
            DEBUG("inc b\n");
            INC(b);
            cpu_cycles = 4;
            break;

        /* DEC B */
        case 0x05:
            DEBUG("dec b\n");
            DEC(b);
            cpu_cycles = 4;
            break;

        /* LD B, n */
        case 0x06:
        {
            b = read_byte();
            DEBUG("ld b, 0x%hhx\n", b);
            cpu_cycles = 8;
            break;
        }

        /* RLCA */
        case 0x07:
            DEBUG("rlca\n");
            RLC(a);
            cpu_cycles = 4;
            break;

        /* LD (nn), SP */
        case 0x08:
        {
            unsigned short tmp = read_word();
            DEBUG("ld (0x%hx), sp)\n", tmp);
            set_mem16(tmp, sp);
            cpu_cycles = 20;
            break;
        }

        /* ADD HL, BC */
        case 0x09:
            DEBUG("add hl, bc\n");
            ADD16(HL, BC);
            cpu_cycles = 8;
            break;

        /* 10 prefix */
        case 0x10:
            op = read_byte();

            switch ( op )
            {
                /* STOP */
                case 0x00:
                    DEBUG("stop\n");
                    /* XXX */
                    cpu_cycles = 4;
                    break;

                default:
                    goto INVALID_OP;
                    break;
            }
            break;

        /* LD A, (BC) */
        case 0x0a:
            DEBUG("ld a, (bc)\n");
            a = get_mem8(GET_BC());
            cpu_cycles = 8;
            break;

        /* DEC BC */
        case 0x0b:
            DEBUG("dec bc\n");
            DEC16(BC);
            cpu_cycles = 8;
            break;

        /* INC C */
        case 0x0c:
            DEBUG("inc c\n");
            INC(c);
            cpu_cycles = 4;
            break;

        /* DEC C */
        case 0x0d:
            DEBUG("dec c\n");
            DEC(c);
            cpu_cycles = 4;
            break;

        /* LD C, n */
        case 0x0e:
            c = read_byte();
            DEBUG("ld c, 0x%hhx\n", c);
            cpu_cycles = 8;
            break;

        /* RRCA */
        case 0x0f:
            DEBUG("rrca\n");
            COND_FLAG(C, (a & 1));
            a = ROR(a);
            COND_FLAG(Z, (a == 0));
            CLEAR_FLAG(N);
            CLEAR_FLAG(H);
            cpu_cycles = 4;
            break;

        /* LD DE, nn */
        case 0x11:
        {
            unsigned short tmp = read_word();
            DEBUG("ld de, 0x%hx\n", tmp);
            SET_DE(tmp);
            cpu_cycles = 12;
            break;
        }

        /* LD (DE), A */
        case 0x12:
            DEBUG("ld (de), a\n");
            set_mem16(GET_DE(), a);
            cpu_cycles = 8;
            break;

        /* INC DE */
        case 0x13:
            DEBUG("inc de\n");
            INC16(DE);
            cpu_cycles = 8;
            break;

        /* INC D */
        case 0x14:
            DEBUG("inc d\n");
            INC(d);
            cpu_cycles = 4;
            break;

        /* DEC D */
        case 0x15:
            DEBUG("dec d\n");
            DEC(d);
            cpu_cycles = 4;
            break;

        /* LD D, n */
        case 0x16:
            d = read_byte();
            DEBUG("ld d, 0x%hhx\n", d);
            cpu_cycles = 8;
            break;

        /* RLA */
        case 0x17:
            DEBUG("rla\n");
            RL(a);
            cpu_cycles = 4;
            break;

        /* JR n */
        case 0x18:
        {
            char tmp = read_byte();
            DEBUG("jr 0x%hhx\n", tmp);
            pc += tmp;
            cpu_cycles = 8;
            break;
        }

        /* ADD HL, DE */
        case 0x19:
            DEBUG("add hl, de\n");
            ADD16(HL, DE);
            cpu_cycles = 8;
            break;

        /* LD A, (DE) */
        case 0x1a:
            DEBUG("ld a, (de)\n");
            a = get_mem8(GET_DE());
            cpu_cycles = 8;
            break;

        /* DEC DE */
        case 0x1b:
            DEBUG("dec de\n");
            DEC16(DE);
            cpu_cycles = 8;
            break;

        /* INC E */
        case 0x1c:
            DEBUG("inc e\n");
            INC(e);
            cpu_cycles = 4;
            break;

        /* DEC E */
        case 0x1d:
            DEBUG("dec e\n");
            DEC(e);
            cpu_cycles = 4;
            break;

        /* LD E, n */
        case 0x1e:
            e = read_byte();
            DEBUG("ld e, 0x%hhx\n", e);
            cpu_cycles = 8;
            break;

        /* RRA */
        case 0x1f:
        {
            char tmp = TEST_FLAG(C);
            DEBUG("rra\n");
            COND_FLAG(C, (a & 0x1));
            a >>= 1;
            a |= (tmp << 7);
            COND_FLAG(Z, (a == 0));
            CLEAR_FLAG(N);
            CLEAR_FLAG(H);
            cpu_cycles = 4;
            break;
        }

        /* JR NZ, * */
        case 0x20:
        {
            char tmp = read_byte();
            DEBUG("jr nz, 0x%hhx\n", tmp);
            if ( ! TEST_FLAG(Z) )
                pc += tmp;
            cpu_cycles = 8;
            break;
        }

        /* LD HL, nn */
        case 0x21:
        {
            short tmp = read_word();
            DEBUG("ld hl, 0x%hx\n", tmp);
            SET_HL(tmp);
            cpu_cycles = 12;
            break;
        }

        /* LDI (HL), A */
        case 0x22:
            DEBUG("ldi (hl), a\n");
            set_mem8(GET_HL(), a);
            SET_HL(GET_HL() + 1);
            cpu_cycles = 8;
            break;

        /* INC HL */
        case 0x23:
            DEBUG("inc hl\n");
            SET_HL(GET_HL() + 1);
            cpu_cycles = 8;
            break;

        /* INC H */
        case 0x24:
            DEBUG("inc h\n");
            INC(h);
            cpu_cycles = 4;
            break;

        /* DEC H */
        case 0x25:
            DEBUG("dec h\n");
            DEC(h);
            cpu_cycles = 4;
            break;

        /* LD H, n */
        case 0x26:
            h = read_byte();
            DEBUG("ld h, 0x%hhx\n", h);
            cpu_cycles = 8;
            break;

        /* DAA */
        case 0x27:
            DEBUG("daa\n");
            /* XXX */
            cpu_cycles = 4;
            break;

        /* JR Z, n */
        case 0x28:
        {
            char tmp = read_byte();
            DEBUG("jr z, %hhx\n", tmp);
            if ( TEST_FLAG(Z) )
                pc += tmp;
            cpu_cycles = 8;
            break;
        }

        /* ADD HL, HL */
        case 0x29:
            DEBUG("add hl, hl\n");
            ADD16(HL, HL);
            cpu_cycles = 8;
            break;

        /* LDI A, (HL) */
        case 0x2a:
            DEBUG("ldi a, (hl)\n");
            a = get_mem8(GET_HL());
            SET_HL(GET_HL() + 1);
            cpu_cycles = 8;
            break;

        /* DEC HL */
        case 0x2b:
            DEBUG("dec hl\n");
            DEC16(HL);
            cpu_cycles = 8;
            break;

        /* INC L */
        case 0x2c:
            DEBUG("inc l\n");
            INC(l);
            cpu_cycles = 4;
            break;

        /* DEC L */
        case 0x2d:
            DEBUG("dec l\n");
            DEC(l);
            cpu_cycles = 4;
            break;

        /* LD L, n */
        case 0x2e:
        {
            l = read_byte();
            DEBUG("ld l, 0x%hhx\n", l);
            cpu_cycles = 8;
            break;
        }

        /* CPL */
        case 0x2f:
            DEBUG("cpl\n");
            a = ~a;
            SET_FLAG(N);
            SET_FLAG(H);
            cpu_cycles = 4;
            break;

        /* JR NC, n */
        case 0x30:
        {
            char tmp = read_byte();
            DEBUG("jr nc, 0x%hhx\n", tmp);
            if ( ! TEST_FLAG(C) )
                pc += tmp;
            cpu_cycles = 8;
            break;
        }

        /* LD SP, nn */
        case 0x31:
        {
            unsigned short tmp = read_word();
            DEBUG("ld sp, 0x%hx\n", tmp);
            sp = tmp;
            cpu_cycles = 12;
            break;
        }

        /* LDD (HL), A */
        case 0x32:
            DEBUG("ldd (hl), a\n");
            set_mem8(GET_HL(), a);
            SET_HL(GET_HL() - 1);
            cpu_cycles = 8;
            break;

        /* INC SP */
        case 0x33:
            DEBUG("inc sp\n");
            sp++;
            cpu_cycles = 8;
            break;

        /* INC (HL) */
        case 0x34:
            DEBUG("inc (hl)\n");
            set_mem8(GET_HL(), get_mem8(GET_HL()) + 1);
            COND_FLAG(Z, (get_mem8(GET_HL()) == 0));
            CLEAR_FLAG(N);
            COND_FLAG(H, HALFCARRY8((get_mem8(GET_HL()) - 1), 1, get_mem8(GET_HL())));
            cpu_cycles = 12;
            break;

        /* DEC (HL) */
        case 0x35:
            DEBUG("dec (hl)\n");
            set_mem8(GET_HL(), get_mem8(GET_HL()) - 1);
            COND_FLAG(Z, (get_mem8(GET_HL()) == 0));
            SET_FLAG(N);
            COND_FLAG(H, !HALFBORROW8((get_mem8(GET_HL()) + 1), 1, get_mem8(GET_HL())));
            cpu_cycles = 12;
            break;

        /* LD (HL), n */
        case 0x36:
        {
            unsigned char tmp = read_byte();
            DEBUG("ld (hl), 0x%hhx\n", tmp);
            set_mem8(GET_HL(), tmp);
            cpu_cycles = 12;
            break;
        }

        /* SCF */
        case 0x37:
            DEBUG("scf\n");
            CLEAR_FLAG(N);
            CLEAR_FLAG(H);
            SET_FLAG(C);
            cpu_cycles = 4;
            break;

        /* JR C, n */
        case 0x38:
        {
            char tmp = read_byte();
            DEBUG("jr c, 0x%hhx\n", tmp);
            if ( TEST_FLAG(C) )
                pc += tmp;
            cpu_cycles = 8;
            break;
        }

        /* ADD HL, SP */
        case 0x39:
            DEBUG("add hl, sp\n");
            ADD16(HL, SP);
            cpu_cycles = 8;
            break;

        /* LDD A, (HL) */
        case 0x3a:
            DEBUG("ldd a, (hl)\n");
            a = get_mem8(GET_HL());
            SET_HL(GET_HL() - 1);
            cpu_cycles = 8;
            break;

        /* DEC SP */
        case 0x3b:
            DEBUG("dec sp\n");
            sp--;
            cpu_cycles = 8;
            break;

        /* INC A */
        case 0x3c:
            DEBUG("inc a\n");
            INC(a);
            cpu_cycles = 4;
            break;

        /* DEC A */
        case 0x3d:
            DEBUG("dec a\n");
            DEC(a);
            cpu_cycles = 4;
            break;

        /* LD A, n */
        case 0x3e:
            a = read_byte();
            DEBUG("ld a, 0x%hhx\n", a);
            cpu_cycles = 8;
            break;

        /* CCF */
        case 0x3f:
            DEBUG("ccf\n");
            CLEAR_FLAG(N);
            CLEAR_FLAG(H);
            COND_FLAG(C, !TEST_FLAG(C));
            cpu_cycles = 4;
            break;

        /* LD B, B */
        case 0x40:
            DEBUG("ld b, b\n");
            cpu_cycles = 4;
            break;

        /* LD B, C */
        case 0x41:
            DEBUG("ld b, c\n");
            b = c;
            cpu_cycles = 4;
            break;

        /* LD B, D */
        case 0x42:
            DEBUG("ld b, d\n");
            b = d;
            cpu_cycles = 4;
            break;

        /* LD B, E */
        case 0x43:
            DEBUG("ld b, e\n");
            b = e;
            cpu_cycles = 4;
            break;

        /* LD B, H */
        case 0x44:
            DEBUG("ld b, h\n");
            b = h;
            cpu_cycles = 4;
            break;

        /* LD B, L */
        case 0x45:
            DEBUG("ld b, l\n");
            b = l;
            cpu_cycles = 4;
            break;

        /* LD B, (HL) */
        case 0x46:
            DEBUG("ld b, (hl)\n");
            b = get_mem8(GET_HL());
            cpu_cycles = 8;
            break;

        /* LD B, A */
        case 0x47:
            DEBUG("ld b, a\n");
            b = a;
            cpu_cycles = 4;
            break;

        /* LD C, B */
        case 0x48:
            DEBUG("ld c, b\n");
            c = b;
            cpu_cycles = 4;
            break;

        /* LD C, C */
        case 0x49:
            DEBUG("ld c, c\n");
            cpu_cycles = 4;
            break;

        /* LD C, D */
        case 0x4a:
            DEBUG("ld c, d\n");
            c = d;
            cpu_cycles = 4;
            break;

        /* LD C, E */
        case 0x4b:
            DEBUG("ld c, e\n");
            c = e;
            cpu_cycles = 4;
            break;

        /* LD C, H */
        case 0x4c:
            DEBUG("ld c, h\n");
            c = h;
            cpu_cycles = 4;
            break;

        /* LD C, L */
        case 0x4d:
            DEBUG("ld c, l\n");
            c = l;
            cpu_cycles = 4;
            break;

        /* LD C, (HL) */
        case 0x4e:
            DEBUG("ld c, (hl)\n");
            c = get_mem8(GET_HL());
            cpu_cycles = 8;
            break;

        /* LD C, A */
        case 0x4f:
            DEBUG("ld c, a\n");
            c = a;
            cpu_cycles = 4;
            break;

        /* LD D, B */
        case 0x50:
            DEBUG("ld d, b\n");
            d = b;
            cpu_cycles = 4;
            break;

        /* LD D, C */
        case 0x51:
            DEBUG("ld d, c\n");
            d = c;
            cpu_cycles = 4;
            break;

        /* LD D, D */
        case 0x52:
            DEBUG("ld d, d\n");
            cpu_cycles = 4;
            break;

        /* LD D, E */
        case 0x53:
            DEBUG("ld d, e\n");
            d = e;
            cpu_cycles = 4;
            break;

        /* LD D, H */
        case 0x54:
            DEBUG("ld d, h\n");
            d = h;
            cpu_cycles = 4;
            break;

        /* LD D, L */
        case 0x55:
            DEBUG("ld d, l\n");
            d = l;
            cpu_cycles = 4;
            break;

        /* LD D, (HL) */
        case 0x56:
            DEBUG("ld d, (hl)\n");
            d = get_mem8(GET_HL());
            cpu_cycles = 8;
            break;

        /* LD D, A */
        case 0x57:
            DEBUG("ld d, a\n");
            d = a;
            cpu_cycles = 4;
            break;

        /* LD E, B */
        case 0x58:
            DEBUG("ld e, b\n");
            e = b;
            cpu_cycles = 4;
            break;

        /* LD E, C */
        case 0x59:
            DEBUG("e, c\n");
            e = c;
            cpu_cycles = 4;
            break;

        /* LD E, D */
        case 0x5a:
            DEBUG("ld e, d\n");
            e = d;
            cpu_cycles = 4;
            break;

        /* LD E, E */
        case 0x5b:
            DEBUG("ld e, e\n");
            cpu_cycles = 4;
            break;

        /* LD E, H */
        case 0x5c:
            DEBUG("ld e, h\n");
            e = h;
            cpu_cycles = 4;
            break;

        /* LD E, L */
        case 0x5d:
            DEBUG("ld e, l\n");
            e = l;
            cpu_cycles = 4;
            break;

        /* LD E, (HL) */
        case 0x5e:
            DEBUG("ld e, (hl)\n");
            e = get_mem8(GET_HL());
            cpu_cycles = 8;
            break;

        /* LD E, A */
        case 0x5f:
            DEBUG("ld e, a\n");
            e = a;
            cpu_cycles = 4;
            break;

        /* LD H, B */
        case 0x60:
            DEBUG("ld h, b\n");
            h = b;
            cpu_cycles = 4;
            break;

        /* LD H, C */
        case 0x61:
            DEBUG("ld h, c\n");
            h = c;
            cpu_cycles = 4;
            break;

        /* LD H, D */
        case 0x62:
            DEBUG("ld h, d\n");
            h = d;
            cpu_cycles = 4;
            break;

        /* LD H, E */
        case 0x63:
            DEBUG("ld h, e\n");
            h = e;
            cpu_cycles = 4;
            break;

        /* LD H, H */
        case 0x64:
            DEBUG("ld h, h\n");
            cpu_cycles = 4;
            break;

        /* LD H, L */
        case 0x65:
            DEBUG("ld h, l\n");
            h = l;
            cpu_cycles = 4;
            break;

        /* LD H, (HL) */
        case 0x66:
            DEBUG("ld h, (hl)\n");
            h = get_mem8(GET_HL());
            cpu_cycles = 8;
            break;

        /* LD H, A */
        case 0x67:
            DEBUG("ld h, a\n");
            h = a;
            cpu_cycles = 4;
            break;

        /* LD L, B */
        case 0x68:
            DEBUG("ld l, b\n");
            l = b;
            cpu_cycles = 4;
            break;

        /* LD L, C */
        case 0x69:
            DEBUG("ld l, c\n");
            l = c;
            cpu_cycles = 4;
            break;

        /* LD L, D */
        case 0x6a:
            DEBUG("ld l, d\n");
            l = d;
            cpu_cycles = 4;
            break;

        /* LD L, E */
        case 0x6b:
            DEBUG("ld l, e\n");
            l = e;
            cpu_cycles = 4;
            break;

        /* LD L, H */
        case 0x6c:
            DEBUG("ld l, h\n");
            l = h;
            cpu_cycles = 4;
            break;

        /* LD L, L */
        case 0x6d:
            DEBUG("ld l, l\n");
            cpu_cycles = 4;
            break;

        /* LD L, (HL) */
        case 0x6e:
            DEBUG("ld l, (hl)\n");
            l = get_mem8(GET_HL());
            cpu_cycles = 8;
            break;

        /* LD L, A */
        case 0x6f:
            DEBUG("ld l, a\n");
            l = a;
            cpu_cycles = 4;
            break;

        /* LD (HL), B */
        case 0x70:
            DEBUG("ld (hl), b\n");
            set_mem8(GET_HL(), b);
            cpu_cycles = 8;
            break;

        /* LD (HL), C */
        case 0x71:
            DEBUG("ld (hl), c\n");
            set_mem8(GET_HL(), c);
            cpu_cycles = 8;
            break;

        /* LD (HL), D */
        case 0x72:
            DEBUG("ld (hl), d\n");
            set_mem8(GET_HL(), d);
            cpu_cycles = 8;
            break;

        /* LD (HL), E */
        case 0x73:
            DEBUG("ld (hl), e\n");
            set_mem8(GET_HL(), e);
            cpu_cycles = 8;
            break;

        /* LD (HL), H */
        case 0x74:
            DEBUG("ld (hl), h\n");
            set_mem8(GET_HL(), h);
            cpu_cycles = 8;
            break;

        /* LD (HL), L */
        case 0x75:
            DEBUG("ld (hl), l\n");
            set_mem8(GET_HL(), l);
            cpu_cycles = 8;
            break;

        /* HALT */
        case 0x76:
            DEBUG("halt\n");
            /* XXX */
            cpu_cycles = 4;
            break;

        /* LD (HL), A */
        case 0x77:
            DEBUG("ld (hl), a\n");
            set_mem8(GET_HL(), a);
            cpu_cycles = 8;
            break;

        /* LD A, B */
        case 0x78:
            DEBUG("ld a, b\n");
            a = b;
            cpu_cycles = 4;
            break;

        /* LD A, C */
        case 0x79:
            DEBUG("ld a, c\n");
            a = c;
            cpu_cycles = 4;
            break;

        /* LD A, D */
        case 0x7a:
            DEBUG("ld a, d\n");
            a = d;
            cpu_cycles = 4;
            break;

        /* LD A, E */
        case 0x7b:
            DEBUG("ld a, e\n");
            a = e;
            cpu_cycles = 4;
            break;

        /* LD A, H */
        case 0x7c:
            DEBUG("ld a, h\n");
            a = h;
            cpu_cycles = 4;
            break;

        /* LD A, L */
        case 0x7d:
            DEBUG("ld a, l\n");
            a = l;
            cpu_cycles = 4;
            break;

        /* LD A, (HL) */
        case 0x7e:
            DEBUG("ld a, (hl)\n");
            a = get_mem8(GET_HL());
            cpu_cycles = 8;
            break;

        /* LD A, A */
        case 0x7f:
            DEBUG("ld a, a\n");
            cpu_cycles = 4;
            break;

        /* ADD A, B */
        case 0x80:
            DEBUG("add a, b\n");
            ADD(a, b);
            cpu_cycles = 4;
            break;

        /* ADD A, C */
        case 0x81:
            DEBUG("add a, c\n");
            ADD(a, c);
            cpu_cycles = 4;
            break;

        /* ADD A, D */
        case 0x82:
            DEBUG("add a, d\n");
            ADD(a, d);
            cpu_cycles = 4;
            break;

        /* ADD A, E */
        case 0x83:
            DEBUG("add a, e\n");
            ADD(a, e);
            cpu_cycles = 4;
            break;

        /* ADD A, H */
        case 0x84:
            DEBUG("add a, h\n");
            ADD(a, h);
            cpu_cycles = 4;
            break;

        /* ADD A, L */
        case 0x85:
            DEBUG("add a, l\n");
            ADD(a, l);
            cpu_cycles = 4;
            break;

        /* ADD A, (HL) */
        case 0x86:
            DEBUG("add a, (hl)\n");
            ADD(a, get_mem8(GET_HL()));
            cpu_cycles = 8;
            break;

        /* ADD A, A */
        case 0x87:
            DEBUG("add a, a\n");
            ADD(a, a);
            cpu_cycles = 4;
            break;

        /* ADC A, B */
        case 0x88:
            DEBUG("adc a, b\n");
            ADC(a, b);
            cpu_cycles = 4;
            break;

        /* ADC A, C */
        case 0x89:
            DEBUG("adc a, c\n");
            ADC(a, c);
            cpu_cycles = 4;
            break;

        /* ADC A, D */
        case 0x8a:
            DEBUG("adc a, d\n");
            ADC(a, d);
            cpu_cycles = 4;
            break;

        /* ADC A, E */
        case 0x8b:
            DEBUG("adc a, e\n");
            ADC(a, e);
            cpu_cycles = 4;
            break;

        /* ADC A, H */
        case 0x8c:
            DEBUG("adc a, h\n");
            ADC(a, h);
            cpu_cycles = 4;
            break;

        /* ADC A, L */
        case 0x8d:
            DEBUG("adc a, l\n");
            ADC(a, l);
            cpu_cycles = 4;
            break;

        /* ADC A, (HL) */
        case 0x8e:
            DEBUG("adc a, (hl)\n");
            ADC(a, get_mem8(GET_HL()));
            cpu_cycles = 8;
            break;

        /* ADC A, A */
        case 0x8f:
            DEBUG("adc a, a\n");
            ADC(a, a);
            cpu_cycles = 4;
            break;

        /* SUB B */
        case 0x90:
            DEBUG("sub b\n");
            SUB(a, b);
            cpu_cycles = 4;
            break;

        /* SUB C */
        case 0x91:
            DEBUG("sub c\n");
            SUB(a, c);
            cpu_cycles = 4;
            break;

        /* SUB D */
        case 0x92:
            DEBUG("sub d\n");
            SUB(a, d);
            cpu_cycles = 4;
            break;

        /* SUB E */
        case 0x93:
            DEBUG("sub e\n");
            SUB(a, e);
            cpu_cycles = 4;
            break;

        /* SUB H */
        case 0x94:
            DEBUG("sub h\n");
            SUB(a, h);
            cpu_cycles = 4;
            break;

        /* SUB L */
        case 0x95:
            DEBUG("sub l\n");
            SUB(a, l);
            cpu_cycles = 4;
            break;

        /* SUB (HL) */
        case 0x96:
            DEBUG("sub (hl)\n");
            SUB(a, get_mem8(GET_HL()));
            cpu_cycles = 8;
            break;

        /* SUB A */
        case 0x97:
            DEBUG("sub a\n");
            SUB(a, a);
            cpu_cycles = 4;
            break;

        /* SBC A, B */
        case 0x98:
            DEBUG("sbc a, b\n");
            SBC(a, b);
            cpu_cycles = 4;
            break;

        /* SBC A, C */
        case 0x99:
            DEBUG("sbc a, c\n");
            SBC(a, c);
            cpu_cycles = 4;
            break;

        /* SBC A, D */
        case 0x9a:
            DEBUG("sbc a, d\n");
            SBC(a, d);
            cpu_cycles = 4;
            break;

        /* SBC A, E */
        case 0x9b:
            DEBUG("sbc a, e\n");
            SBC(a, e);
            cpu_cycles = 4;
            break;

        /* SBC A, H */
        case 0x9c:
            DEBUG("sbc a, h\n");
            SBC(a, h);
            cpu_cycles = 4;
            break;

        /* SBC A, L */
        case 0x9d:
            DEBUG("a, l\n");
            SBC(a, h);
            cpu_cycles = 4;
            break;

        /* SBC A, (HL) */
        case 0x9e:
            DEBUG("sbc a, (hl)\n");
            SBC(a, get_mem8(GET_HL()));
            cpu_cycles = 8;
            break;

        /* SBC A, A */
        case 0x9f:
            DEBUG("sbc a, a\n");
            SBC(a, a);
            cpu_cycles = 4;
            break;

        /* AND B */
        case 0xa0:
            DEBUG("and b\n");
            BITWISE(a, b, &=, 1);
            cpu_cycles = 4;
            break;

        /* AND C */
        case 0xa1:
            DEBUG("and c\n");
            BITWISE(a, c, &=, 1);
            cpu_cycles = 4;
            break;

        /* AND D */
        case 0xa2:
            DEBUG("and d\n");
            BITWISE(a, d, &=, 1);
            cpu_cycles = 4;
            break;

        /* AND E */
        case 0xa3:
            DEBUG("and e\n");
            BITWISE(a, e, &=, 1);
            cpu_cycles = 4;
            break;

        /* AND H */
        case 0xa4:
            DEBUG("and h\n");
            BITWISE(a, h, &=, 1);
            cpu_cycles = 4;
            break;

        /* AND L */
        case 0xa5:
            DEBUG("and l\n");
            BITWISE(a, l, &=, 1);
            cpu_cycles = 4;
            break;

        /* AND (HL) */
        case 0xa6:
            DEBUG("and (hl)\n");
            BITWISE(a, get_mem8(GET_HL()), &=, 1);
            cpu_cycles = 8;
            break;

        /* AND A */
        case 0xa7:
            DEBUG("and a\n");
            BITWISE(a, a, &=, 1);
            cpu_cycles = 4;
            break;

        /* XOR B */
        case 0xa8:
            DEBUG("xor b\n");
            BITWISE(a, b, ^=, 0);
            cpu_cycles = 4;
            break;

        /* XOR C */
        case 0xa9:
            DEBUG("xor c\n");
            BITWISE(a, c, ^=, 0);
            cpu_cycles = 4;
            break;

        /* XOR D */
        case 0xaa:
            DEBUG("xor d\n");
            BITWISE(a, d, ^=, 0);
            cpu_cycles = 4;
            break;

        /* XOR E */
        case 0xab:
            DEBUG("xor e\n");
            BITWISE(a, e, ^=, 0);
            cpu_cycles = 4;
            break;

        /* XOR H */
        case 0xac:
            DEBUG("xor h\n");
            BITWISE(a, h, ^=, 0);
            cpu_cycles = 4;
            break;

        /* XOR L */
        case 0xad:
            DEBUG("xor l\n");
            BITWISE(a, l, ^=, 0);
            cpu_cycles = 4;
            break;

        /* XOR (HL) */
        case 0xae:
            DEBUG("xor (hl)\n");
            BITWISE(a, get_mem8(GET_HL()), ^=, 0);
            cpu_cycles = 8;
            break;

        /* XOR A */
        case 0xaf:
            DEBUG("xor a\n");
            BITWISE(a, a, ^=, 0);
            cpu_cycles = 4;
            break;

        /* OR B */
        case 0xb0:
            DEBUG("or b\n");
            BITWISE(a, b, |=, 0);
            cpu_cycles = 4;
            break;

        /* OR C */
        case 0xb1:
            DEBUG("or c\n");
            BITWISE(a, c, |=, 0);
            cpu_cycles = 4;
            break;

        /* OR D */
        case 0xb2:
            DEBUG("or d\n");
            BITWISE(a, d, |=, 0);
            cpu_cycles = 4;
            break;

        /* OR E */
        case 0xb3:
            DEBUG("or e\n");
            BITWISE(a, e, |=, 0);
            cpu_cycles = 4;
            break;

        /* OR H */
        case 0xb4:
            DEBUG("or h\n");
            BITWISE(a, h, |=, 0);
            cpu_cycles = 4;
            break;

        /* OR L */
        case 0xb5:
            DEBUG("or l\n");
            BITWISE(a, l, |=, 0);
            cpu_cycles = 4;
            break;

        /* OR (HL) */
        case 0xb6:
            DEBUG("or (hl)\n");
            BITWISE(a, get_mem8(GET_HL()), |=, 0);
            cpu_cycles = 8;
            break;

        /* OR A */
        case 0xb7:
            DEBUG("or a\n");
            BITWISE(a, a, |=, 0);
            cpu_cycles = 4;
            break;

        /* CP B */
        case 0xb8:
            DEBUG("cp b\n");
            COMP(a, b);
            cpu_cycles = 4;
            break;

        /* CP C */
        case 0xb9:
            DEBUG("cp c\n");
            COMP(a, c);
            cpu_cycles = 4;
            break;

        /* CP D */
        case 0xba:
            DEBUG("cp d\n");
            COMP(a, d);
            cpu_cycles = 4;
            break;

        /* CP E */
        case 0xbb:
            DEBUG("cp e\n");
            COMP(a, e);
            cpu_cycles = 4;
            break;

        /* CP H */
        case 0xbc:
            DEBUG("cp h\n");
            COMP(a, h);
            cpu_cycles = 4;
            break;

        /* CP L */
        case 0xbd:
            DEBUG("cp l\n");
            COMP(a, l);
            cpu_cycles = 4;
            break;

        /* CP (HL) */
        case 0xbe:
            DEBUG("cp (hl)\n");
            COMP(a, get_mem8(GET_HL()));
            cpu_cycles = 8;
            break;

        /* CP A */
        case 0xbf:
            DEBUG("cp a\n");
            COMP(a, a);
            cpu_cycles = 4;
            break;

        /* RET NZ */
        case 0xc0:
            DEBUG("ret nz\n");
            if ( ! TEST_FLAG(Z) )
                RET();
            cpu_cycles = 8;
            break;

        /* POP BC */
        case 0xc1:
            DEBUG("pop bc\n");
            POP(BC);
            cpu_cycles = 12;
            break;

        /* JP NZ, nn */
        case 0xc2:
        {
            unsigned short tmp = read_word();
            DEBUG("jp nz, 0x%hx\n", tmp);
            if ( ! TEST_FLAG(Z) )
                pc = tmp;
            cpu_cycles = 12;
            break;
        }

        /* JP nn */
        case 0xc3:
            pc = read_word();
            DEBUG("jp 0x%hx\n", pc);
            cpu_cycles = 12;
            break;

        /* CALL NZ, nn */
        case 0xc4:
        {
            unsigned short tmp = read_word();
            DEBUG("call nz, 0x%hx\n", tmp);
            if ( ! TEST_FLAG(Z) )
                CALL(tmp);
            cpu_cycles = 12;
            break;
        }

        /* PUSH BC */
        case 0xc5:
            DEBUG("push bc\n");
            PUSH(GET_BC());
            cpu_cycles = 16;
            break;

        /* ADD A, n */
        case 0xc6:
        {
            unsigned char tmp = read_byte();
            DEBUG("add a, 0x%hhx\n", tmp);
            ADD(a, tmp);
            cpu_cycles = 8;
            break;
        }

        /* RST 00H */
        case 0xc7:
            DEBUG("RST 0x0\n");
            CALL(0x0);
            cpu_cycles = 32;
            break;

        /* RET Z */
        case 0xc8:
            DEBUG("ret z\n");
            if ( TEST_FLAG(Z) )
                RET();
            cpu_cycles = 8;
            break;

        /* RET */
        case 0xc9:
            DEBUG("ret\n");
            RET();
            cpu_cycles = 8;
            break;

        /* JP Z, nn */
        case 0xca:
        {
            unsigned short tmp = read_word();
            DEBUG("jp z, 0x%hx\n", tmp);
            if ( TEST_FLAG(Z) )
                pc = tmp;
            cpu_cycles = 12;
            break;
        }

        /* CB prefix */
        case 0xcb:
            op = read_byte();

            switch ( op )
            {
                /* RLC B */
                case 0x00:
                    DEBUG("rlc b\n");
                    RLC(b);
                    cpu_cycles = 8;
                    break;

                /* RLC C */
                case 0x01:
                    DEBUG("rlc c\n");
                    RLC(c);
                    cpu_cycles = 8;
                    break;

                /* RLC D */
                case 0x02:
                    DEBUG("rlc d\n");
                    RLC(d);
                    cpu_cycles = 8;
                    break;

                /* RLC E */
                case 0x03:
                    DEBUG("rlc e\n");
                    RLC(e);
                    cpu_cycles = 8;
                    break;

                /* RLC H */
                case 0x04:
                    DEBUG("rlc h\n");
                    RLC(h);
                    cpu_cycles = 8;
                    break;

                /* RLC L */
                case 0x05:
                    DEBUG("rlc l\n");
                    RLC(l);
                    cpu_cycles = 8;
                    break;

                /* RLC (HL) */
                case 0x06:
                    DEBUG("rlc (hl)\n");
                    COND_FLAG(C, (get_mem8(GET_HL()) & 0x80));
                    set_mem8(GET_HL(), ROL(get_mem8(GET_HL())));
                    COND_FLAG(Z, (GET_HL() == 0));
                    CLEAR_FLAG(N);
                    CLEAR_FLAG(H);
                    cpu_cycles = 16;
                    break;

                /* RLC A */
                case 0x07:
                    DEBUG("rlc a\n");
                    RLC(a);
                    cpu_cycles = 8;
                    break;

                /* RRC B */
                case 0x08:
                    DEBUG("rrc b\n");
                    RRC(b);
                    cpu_cycles = 8;
                    break;

                /* RRC C */
                case 0x09:
                    DEBUG("rrc c\n");
                    RRC(c);
                    cpu_cycles = 8;
                    break;

                /* RRC D */
                case 0x0a:
                    DEBUG("rrc d\n");
                    RRC(d);
                    cpu_cycles = 8;
                    break;

                /* RRC E */
                case 0x0b:
                    DEBUG("rrc e\n");
                    RRC(e);
                    cpu_cycles = 8;
                    break;

                /* RRC H */
                case 0x0c:
                    DEBUG("rrc h\n");
                    RRC(h);
                    cpu_cycles = 8;
                    break;

                /* RRC L */
                case 0x0d:
                    DEBUG("rrc l\n");
                    RRC(l);
                    cpu_cycles = 8;
                    break;

                /* RRC (HL) */
                case 0x0e:
                    DEBUG("rrc (hl)\n");
                    COND_FLAG(C, (get_mem8(GET_HL()) & 1));
                    set_mem8(GET_HL(), ROR(get_mem8(GET_HL())));
                    COND_FLAG(Z, (GET_HL() == 0));
                    CLEAR_FLAG(N);
                    CLEAR_FLAG(H);
                    cpu_cycles = 16;
                    break;

                /* RRC A */
                case 0x0f:
                    DEBUG("rrc a\n");
                    RRC(a);
                    cpu_cycles = 8;
                    break;

                /* RL B */
                case 0x10:
                    DEBUG("rl b\n");
                    RL(b);
                    cpu_cycles = 8;
                    break;

                /* RL C */
                case 0x11:
                    DEBUG("rl c\n");
                    RL(c);
                    cpu_cycles = 8;
                    break;

                /* RL D */
                case 0x12:
                    DEBUG("rl d\n");
                    RL(d);
                    cpu_cycles = 8;
                    break;

                /* RL E */
                case 0x13:
                    DEBUG("rl e\n");
                    RL(e);
                    cpu_cycles = 8;
                    break;

                /* RL H */
                case 0x14:
                    DEBUG("rl h\n");
                    RL(h);
                    cpu_cycles = 8;
                    break;

                /* RL L */
                case 0x15:
                    DEBUG("rl l\n");
                    RL(l);
                    cpu_cycles = 8;
                    break;

                /* RL (HL) */
                case 0x16:
                {
                    unsigned char tmp = TEST_FLAG(C);
                    DEBUG("rl (hl)\n");
                    COND_FLAG(C, (get_mem8(GET_HL()) & 0x80));
                    set_mem8(GET_HL(), ((get_mem8(GET_HL()) << 1) | tmp));
                    COND_FLAG(Z, (get_mem8(GET_HL()) == 0));
                    CLEAR_FLAG(N);
                    CLEAR_FLAG(H);
                    cpu_cycles = 16;
                    break;
                }

                /* RL A */
                case 0x17:
                    DEBUG("rl a\n");
                    RL(a);
                    cpu_cycles = 8;
                    break;

                /* RR B */
                case 0x18:
                    DEBUG("rr b\n");
                    RR(b);
                    cpu_cycles = 8;
                    break;

                /* RR C */
                case 0x19:
                    DEBUG("rr c\n");
                    RR(c);
                    cpu_cycles = 8;
                    break;

                /* RR D */
                case 0x1a:
                    DEBUG("rr d\n");
                    RR(d);
                    cpu_cycles = 8;
                    break;

                /* RR E */
                case 0x1b:
                    DEBUG("rr e\n");
                    RR(e);
                    cpu_cycles = 8;
                    break;

                /* RR H */
                case 0x1c:
                    DEBUG("rr h\n");
                    RR(h);
                    cpu_cycles = 8;
                    break;

                /* RR L */
                case 0x1d:
                    DEBUG("rr l\n");
                    RR(l);
                    cpu_cycles = 8;
                    break;

                /* RR (HL) */
                case 0x1e:
                {
                    unsigned char tmp = TEST_FLAG(C);
                    DEBUG("rr (hl)\n");
                    COND_FLAG(C, (get_mem8(GET_HL()) & 0x1));
                    set_mem8(GET_HL(), ((get_mem8(GET_HL()) >> 1) | (tmp << 7)));
                    COND_FLAG(Z, (get_mem8(GET_HL()) == 0));
                    CLEAR_FLAG(N);
                    CLEAR_FLAG(H);
                    cpu_cycles = 16;
                    break;
                }

                /* RR A */
                case 0x1f:
                    DEBUG("rr a\n");
                    RR(a);
                    cpu_cycles = 8;
                    break;

                /* SLA B */
                case 0x20:
                    DEBUG("sla b\n");
                    SLA(b);
                    cpu_cycles = 8;
                    break;

                /* SLA C */
                case 0x21:
                    DEBUG("sla c\n");
                    SLA(c);
                    cpu_cycles = 8;
                    break;

                /* SLA D */
                case 0x22:
                    DEBUG("sla d\n");
                    SLA(d);
                    cpu_cycles = 8;
                    break;

                /* SLA E */
                case 0x23:
                    DEBUG("sla e\n");
                    SLA(e);
                    cpu_cycles = 8;
                    break;

                /* SLA H */
                case 0x24:
                    DEBUG("sla h\n");
                    SLA(h);
                    cpu_cycles = 8;
                    break;

                /* SLA L */
                case 0x25:
                    DEBUG("sla l\n");
                    SLA(l);
                    cpu_cycles = 8;
                    break;

                /* SLA (HL) */
                case 0x26:
                {
                    DEBUG("sla (hl)\n");
                    COND_FLAG(C, (get_mem8(GET_HL()) & 0x80));
                    set_mem8(GET_HL(), (get_mem8(GET_HL()) << 1));
                    COND_FLAG(Z, (get_mem8(GET_HL()) == 0));
                    CLEAR_FLAG(N);
                    CLEAR_FLAG(H);
                    cpu_cycles = 16;
                    break;
                }

                /* SLA A */
                case 0x27:
                    DEBUG("sla a\n");
                    SLA(a);
                    cpu_cycles = 8;
                    break;

                /* SRA B */
                case 0x28:
                    DEBUG("sra b\n");
                    SRA(b);
                    cpu_cycles = 8;
                    break;

                /* SRA C */
                case 0x29:
                    DEBUG("sra c\n");
                    SRA(c);
                    cpu_cycles = 8;
                    break;

                /* SRA D */
                case 0x2a:
                    DEBUG("sra d\n");
                    SRA(d);
                    cpu_cycles = 8;
                    break;

                /* SRA E */
                case 0x2b:
                    DEBUG("sra e\n");
                    SRA(e);
                    cpu_cycles = 8;
                    break;

                /* SRA H */
                case 0x2c:
                    DEBUG("sra h\n");
                    SRA(h);
                    cpu_cycles = 8;
                    break;

                /* SRA L */
                case 0x2d:
                    DEBUG("sra l\n");
                    SRA(l);
                    cpu_cycles = 8;
                    break;

                /* SRA (HL) */
                case 0x2e:
                {
                    DEBUG("sra (hl)\n");
                    COND_FLAG(C, (get_mem8(GET_HL()) & 0x1));
                    set_mem8(GET_HL(), (get_mem8(GET_HL()) >> 1) | (get_mem8(GET_HL()) & 0x80));
                    COND_FLAG(Z, (get_mem8(GET_HL()) == 0));
                    CLEAR_FLAG(N);
                    CLEAR_FLAG(H);
                    cpu_cycles = 16;
                    break;
                }

                /* SRA A */
                case 0x2f:
                    DEBUG("sra a\n");
                    SRA(a);
                    cpu_cycles = 8;
                    break;

                /* SWAP B */
                case 0x30:
                    DEBUG("swap b\n");
                    SWAP(b);
                    cpu_cycles = 8;
                    break;

                /* SWAP C */
                case 0x31:
                    DEBUG("swap c\n");
                    SWAP(c);
                    cpu_cycles = 8;
                    break;

                /* SWAP D */
                case 0x32:
                    DEBUG("swap d\n");
                    SWAP(d);
                    cpu_cycles = 8;
                    break;

                /* SWAP E */
                case 0x33:
                    DEBUG("swap e\n");
                    SWAP(e);
                    cpu_cycles = 8;
                    break;

                /* SWAP H */
                case 0x34:
                    DEBUG("swap h\n");
                    SWAP(h);
                    cpu_cycles = 8;
                    break;

                /* SWAP L */
                case 0x35:
                    DEBUG("swap l\n");
                    SWAP(l);
                    cpu_cycles = 8;
                    break;

                /* SWAP (HL) */
                case 0x36:
                    DEBUG("swap (hl)\n");
                    SET_HL(((GET_HL() & 0xf) << 4) | ((GET_HL() & 0xf0) >> 4));
                    COND_FLAG(Z, (GET_HL() == 0));
                    CLEAR_FLAG(N);
                    CLEAR_FLAG(H);
                    CLEAR_FLAG(C);
                    cpu_cycles = 16;
                    break;

                /* SWAP A */
                case 0x37:
                    DEBUG("swap a\n");
                    SWAP(a);
                    cpu_cycles = 8;
                    break;

                /* SRL B */
                case 0x38:
                    DEBUG("srl b\n");
                    SRL(b);
                    cpu_cycles = 8;
                    break;

                /* SRL C */
                case 0x39:
                    DEBUG("srl c\n");
                    SRL(c);
                    cpu_cycles = 8;
                    break;

                /* SRL D */
                case 0x3a:
                    DEBUG("srl d\n");
                    SRL(d);
                    cpu_cycles = 8;
                    break;

                /* SRL E */
                case 0x3b:
                    DEBUG("srl e\n");
                    SRL(e);
                    cpu_cycles = 8;
                    break;

                /* SRL H */
                case 0x3c:
                    DEBUG("srl h\n");
                    SRL(h);
                    cpu_cycles = 8;
                    break;

                /* SRL L */
                case 0x3d:
                    DEBUG("srl l\n");
                    SRL(l);
                    cpu_cycles = 8;
                    break;

                /* SRL (HL) */
                case 0x3e:
                {
                    DEBUG("srl (hl)\n");
                    COND_FLAG(C, (get_mem8(GET_HL()) & 0x1));
                    set_mem8(GET_HL(), (get_mem8(GET_HL()) >> 1));
                    COND_FLAG(Z, (get_mem8(GET_HL()) == 0));
                    CLEAR_FLAG(N);
                    CLEAR_FLAG(H);
                    cpu_cycles = 16;
                    break;
                }

                /* SRL A */
                case 0x3f:
                    DEBUG("srl a");
                    SRL(a);
                    cpu_cycles = 8;
                    break;

                default:
                {
                    char reg = op & 0x07;
                    char bit = (op & 0x38) >> 3;
                    char ins = (op & 0xc0) >> 6;

                    switch ( ins )
                    {
                        /* BIT b, r */
                        case 0:
                            switch ( reg )
                            {
                                /* BIT b, B */
                                case 0:
                                    DEBUG("bit %hhu, b\n", bit);
                                    BIT(b, bit);
                                    break;

                                /* BIT b, C */
                                case 1:
                                    DEBUG("bit %hhu, c\n", bit);
                                    BIT(c, bit);
                                    break;

                                /* BIT b, D */
                                case 2:
                                    DEBUG("bit %hhu, d\n", bit);
                                    BIT(d, bit);
                                    break;

                                /* BIT b, E */
                                case 3:
                                    DEBUG("bit %hhu, e\n", bit);
                                    BIT(e, bit);
                                    break;

                                /* BIT b, H */
                                case 4:
                                    DEBUG("bit %hhu, h\n", bit);
                                    BIT(h, bit);
                                    break;

                                /* BIT b, L */
                                case 5:
                                    DEBUG("bit %hhu, l\n", bit);
                                    BIT(l, bit);
                                    break;

                                /* BIT b, (HL) */
                                case 6:
                                    DEBUG("bit %hhu, (hl)\n", bit);
                                    BIT(get_mem8(GET_HL()), bit);
                                    cpu_cycles = 8;
                                    break;

                                /* BIT b, A */
                                case 7:
                                    DEBUG("bit %hhu, a\n", bit);
                                    BIT(a, bit);
                                    break;
                            }
                            break;

                        /* RES b, r */
                        case 1:
                            switch ( reg )
                            {
                                /* RES b, B */
                                case 0:
                                    DEBUG("res %hhu, b\n", bit);
                                    b = CLEAR_BIT(b, bit);
                                    break;

                                /* RES b, C */
                                case 1:
                                    DEBUG("res %hhu, c\n", bit);
                                    c = CLEAR_BIT(c, bit);
                                    break;

                                /* RES b, D */
                                case 2:
                                    DEBUG("res %hhu, d\n", bit);
                                    d = CLEAR_BIT(d, bit);
                                    break;

                                /* RES b, E */
                                case 3:
                                    DEBUG("res %hhu, e\n", bit);
                                    e = CLEAR_BIT(e, bit);
                                    break;

                                /* RES b, H */
                                case 4:
                                    DEBUG("res %hhu, h\n", bit);
                                    h = CLEAR_BIT(h, bit);
                                    break;

                                /* RES b, L */
                                case 5:
                                    DEBUG("res %hhu, l\n", bit);
                                    l = CLEAR_BIT(l, bit);
                                    break;

                                /* RES b, (HL) */
                                case 6:
                                    DEBUG("res %hhu, (hl)\n", bit);
                                    set_mem8(GET_HL(), CLEAR_BIT(get_mem8(GET_HL()), bit));
                                    cpu_cycles = 8;
                                    break;

                                /* RES b, A */
                                case 7:
                                    DEBUG("res %hhu, a\n", bit);
                                    a = CLEAR_BIT(a, bit);
                                    break;
                            }
                            break;

                        /* SET b, r */
                        case 2:
                            switch ( reg )
                            {
                                /* SET b, B */
                                case 0:
                                    DEBUG("set %hhu, b\n", bit);
                                    b = SET_BIT(b, bit);
                                    break;

                                /* SET b, C */
                                case 1:
                                    DEBUG("set %hhu, c\n", bit);
                                    c = SET_BIT(c, bit);
                                    break;

                                /* SET b, D */
                                case 2:
                                    DEBUG("set %hhu, d\n", bit);
                                    d = SET_BIT(d, bit);
                                    break;

                                /* SET b, E */
                                case 3:
                                    DEBUG("set %hhu, e\n", bit);
                                    e = SET_BIT(e, bit);
                                    break;

                                /* SET b, H */
                                case 4:
                                    DEBUG("set %hhu, h\n", bit);
                                    h = SET_BIT(h, bit);
                                    break;

                                /* SET b, L */
                                case 5:
                                    DEBUG("set %hhu, l\n", bit);
                                    l = SET_BIT(l, bit);
                                    break;

                                /* SET b, (HL) */
                                case 6:
                                    DEBUG("set %hhu, (hl)\n", bit);
                                    set_mem8(GET_HL(), SET_BIT(get_mem8(GET_HL()), bit));
                                    cpu_cycles = 8;
                                    break;

                                /* SET b, A */
                                case 7:
                                    DEBUG("set %hhu, a\n", bit);
                                    a = SET_BIT(a, bit);
                                    break;
                            }
                            break;
                    }
                    cpu_cycles = 8; // (HL) instructions get an extra 8 cycles above
                }
            }
            break;

        /* CALL Z, nn */
        case 0xcc:
        {
            unsigned short tmp = read_word();
            DEBUG("call z, 0x%hx\n", tmp);
            if ( TEST_FLAG(Z) )
                CALL(tmp);
            cpu_cycles = 12;
            break;
        }

        /* CALL nn */
        case 0xcd:
        {
            unsigned short tmp = read_word();
            DEBUG("call 0x%hx\n", tmp);
            CALL(tmp);
            cpu_cycles = 12;
            break;
        }

        /* ADC A, n */
        case 0xce:
        {
            unsigned char tmp = read_byte();
            DEBUG("adc a, 0x%hhx\n", tmp);
            ADC(a, tmp);
            cpu_cycles = 8;
            break;
        }

        /* RST 08H */
        case 0xcf:
            DEBUG("rst 0x8\n");
            CALL(0x8);
            cpu_cycles = 32;
            break;

        /* RET NC */
        case 0xd0:
            DEBUG("ret nc\n");
            if ( ! TEST_FLAG(C) )
                RET();
            cpu_cycles = 8;
            break;

        /* POP DE */
        case 0xd1:
            DEBUG("pop de\n");
            POP(DE);
            cpu_cycles = 12;
            break;

        /* JP NC, nn */
        case 0xd2:
        {
            unsigned short tmp = read_word();
            DEBUG("jp nc, 0x%hx\n", tmp);
            if ( ! TEST_FLAG(C) )
                pc = tmp;
            cpu_cycles = 12;
            break;
        }

        /* CALL NC, nn */
        case 0xd4:
        {
            unsigned short tmp = read_word();
            DEBUG("call nc, 0x%hx\n", tmp);
            if ( ! TEST_FLAG(C) )
                CALL(tmp);
            cpu_cycles = 12;
            break;
        }

        /* PUSH DE */
        case 0xd5:
            DEBUG("push de\n");
            PUSH(GET_DE());
            cpu_cycles = 16;
            break;

        /* SUB n */
        case 0xd6:
        {
            unsigned char tmp = read_byte();
            DEBUG("sub 0x%hhx\n", tmp);
            SUB(a, tmp);
            cpu_cycles = 8;
            break;
        }

        /* RST 10H */
        case 0xd7:
            DEBUG("rst 0x10\n");
            CALL(0x10);
            cpu_cycles = 32;
            break;

        /* RET C */
        case 0xd8:
            DEBUG("ret c\n");
            if ( TEST_FLAG(C) )
                RET();
            cpu_cycles = 8;
            break;

        /* RETI */
        case 0xd9:
            DEBUG("reti\n");
            ime = 1;
            RET();
            cpu_cycles = 8;
            break;

        /* JP C, nn */
        case 0xda:
        {
            unsigned short tmp = read_word();
            DEBUG("jp c, 0x%hx\n", tmp);
            if ( TEST_FLAG(C) )
                pc = tmp;
            cpu_cycles = 12;
            break;
        }

        /* CALL C, nn */
        case 0xdc:
        {
            unsigned short tmp = read_word();
            DEBUG("call c, 0x%hx\n", tmp);
            if ( TEST_FLAG(C) )
                CALL(tmp);
            cpu_cycles = 12;
            break;
        }

        /* RST 18H */
        case 0xdf:
            DEBUG("rst 0x18\n");
            CALL(0x18);
            cpu_cycles = 32;
            break;

        /* LDH (n), A */
        case 0xe0:
        {
            unsigned char tmp = read_byte();
            DEBUG("ld (0xff00+0x%hhx), a\n", tmp);
            set_mem8(0xff00 | tmp, a);
            cpu_cycles = 12;
            break;
        }

        /* POP HL */
        case 0xe1:
            DEBUG("pop hl\n");
            POP(HL);
            cpu_cycles = 12;
            break;

        /* LD (C), A */
        case 0xe2:
            DEBUG("ld (0xff00+c), a\n");
            set_mem8(0xff00 | c, a);
            cpu_cycles = 8;
            break;

        /* PUSH HL */
        case 0xe5:
            DEBUG("push hl\n");
            PUSH(GET_HL());
            cpu_cycles = 16;
            break;

        /* AND n */
        case 0xe6:
        {
            unsigned char tmp = read_byte();
            DEBUG("and 0x%hhx\n", tmp);
            BITWISE(a, tmp, &=, 1);
            cpu_cycles = 8;
            break;
        }

        /* RST 20H */
        case 0xe7:
            DEBUG("rst 0x20\n");
            CALL(0x20);
            cpu_cycles = 32;
            break;

        /* ADD SP, n */
        case 0xe8:
        {
            unsigned short orig = sp;
            char toadd = read_byte();
            DEBUG("add sp, 0x%hhx\n", toadd);
            sp += toadd;
            CLEAR_FLAG(Z);
            CLEAR_FLAG(N);
            COND_FLAG(H, HALFCARRY16(orig, toadd, sp));
            COND_FLAG(C, CARRY(orig, sp));
            cpu_cycles = 16;
            break;
        }

        /* JP (HL) */
        case 0xe9:
            DEBUG("jp (hl)\n");
            pc = get_mem16(GET_HL());
            cpu_cycles = 4;
            break;

        /* LD (nn), A */
        case 0xea:
        {
            unsigned short tmp = read_word();
            DEBUG("ld (0x%hx), a\n", tmp);
            set_mem16(tmp, a);
            cpu_cycles = 16;
            break;
        }

        /* XOR n */
        case 0xee:
        {
            unsigned char tmp = read_byte();
            DEBUG("xor 0x%hhx\n", tmp);
            BITWISE(a, tmp, ^=, 0);
            cpu_cycles = 8;
            break;
        }

        /* RST 28H */
        case 0xef:
            DEBUG("rst 0x28\n");
            CALL(0x28);
            cpu_cycles = 32;
            break;

        /* LDH A, (n) */
        case 0xf0:
        {
            char tmp = read_byte();
            DEBUG("ld a, (0xff00+0x%hhx)\n", tmp);
            a = get_mem8(0xff00 | tmp);
            cpu_cycles = 12;
            break;
        }

        /* POP AF */
        case 0xf1:
            DEBUG("pop af\n");
            POP(AF);
            cpu_cycles = 12;
            break;

        /* LD A, (C) */
        case 0xf2:
            DEBUG("ld a, (0xff00+c)\n");
            a = get_mem8(0xff00 | c);
            cpu_cycles = 8;
            break;

        /* DI */
        case 0xf3:
            DEBUG("di\n");
            ime = 0;
            cpu_cycles = 4;
            break;

        /* PUSH AF */
        case 0xf5:
            DEBUG("push af\n");
            PUSH(GET_AF());
            cpu_cycles = 16;
            break;

        /* OR n */
        case 0xf6:
        {
            unsigned char tmp = read_byte();
            DEBUG("or 0x%hhx\n", tmp);
            BITWISE(a, tmp, |=, 0);
            cpu_cycles = 8;
            break;
        }

        /* RST 30H */
        case 0xf7:
            DEBUG("rst 0x30\n");
            CALL(0x30);
            cpu_cycles = 32;
            break;

        /* LDHL SP, n */
        case 0xf8:
        {
            unsigned short orig = sp;
            char added = read_byte();
            DEBUG("ldhl sp, 0x%hhx\n", added);
            SET_HL(sp + added);
            CLEAR_FLAG(Z);
            CLEAR_FLAG(N);
            COND_FLAG(H, HALFCARRY16(orig, added, GET_HL()));
            COND_FLAG(C, CARRY(orig, GET_HL()));
            cpu_cycles = 12;
            break;
        }

        /* LD SP, HL */
        case 0xf9:
            DEBUG("ld sp, hl\n");
            sp = GET_HL();
            cpu_cycles = 8;
            break;

        /* LD A, (nn) */
        case 0xfa:
        {
            unsigned short tmp = read_word();
            DEBUG("ld a, (%hx)\n", tmp);
            a = get_mem8(tmp);
            cpu_cycles = 16;
            break;
        }

        /* EI */
        case 0xfb:
            DEBUG("ei\n");
            ime = 1;
            cpu_cycles = 4;
            break;

        /* CP n */
        case 0xfe:
        {
            unsigned char tmp = read_byte();
            DEBUG("cp 0x%hhx (a=%hhx)\n", tmp, a);
            COMP(a, tmp);
            cpu_cycles = 8;
            break;
        }

        /* RST 38H */
        case 0xff:
            DEBUG("rst 0x38\n");
            CALL(0x38);
            cpu_cycles = 32;
            break;

        default:
            goto INVALID_OP;
            break;
    }

    total_cpu_cycles += cpu_cycles;

//    dump_regs();
    return;

INVALID_OP:
    DEBUG("(bad: %hhx)\n", op);
}

void init_cpu ( void )
{
    pc = 0x100;
    halt = 0;
    total_cpu_cycles = 0;
}
