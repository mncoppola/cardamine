int handle_ioregs_read(unsigned short addr, char *value);
int handle_ioregs_write(unsigned short addr, char value);

/*
 * GameBoy I/O Ports
 */

/*
 * Video Display
 */

/* LCD Control Register */
#define LCDC 0xff40

/* LCD Status Register */
#define STAT 0xff41

/* LCD Position and Scrolling */
#define SCY  0xff42
#define SCX  0xff43
#define LY   0xff44
#define LYC  0xff45
#define WY   0xff4a
#define WX   0xff4b

/* LCD Monochrome Palettes */
#define BGP  0xff47
#define OBP0 0xff48
#define OBP1 0xff49

/* LCD OAM DMA Transfers */
#define DMA  0xff46

/*
 * Sound Controller
 */

/* Sound Channel 1 - Tone & Sweep */
#define NR10 0xff10
#define NR11 0xff11
#define NR12 0xff12
#define NR13 0xff13
#define NR14 0xff14

/* Sound Channel 2 - Tone */
#define NR21 0xff16
#define NR22 0xff17
#define NR23 0xff18
#define NR24 0xff19

/* Sound Channel 3 - Wave Output */
#define NR30 0xff1a
#define NR31 0xff1b
#define NR32 0xff1c
#define NR33 0xff1d
#define NR34 0xff1e
#define WAVE_PATTERN_RAM 0xff30

/* Sound Channel 4 - Noise */
#define NR41 0xff20
#define NR42 0xff21
#define NR43 0xff22
#define NR44 0xff23

/* Sound Control Registers */
#define NR50 0xff24
#define NR51 0xff25
#define NR52 0xff26

/*
 * Joypad Input
 */

#define JOYP 0xff00

/*
 * Serial Data Transfer (Link Cable)
 */

#define SB 0xff01
#define SC 0xff02

/*
 * Timer and Divider Registers
 */

#define DIV  0xff04
#define TIMA 0xff05
#define TMA  0xff06
#define TAC  0xff07

/*
 * Boot ROM Disable
 */

#define BOOT 0xff50
