void init_mem(void);
char get_mem8(unsigned short addr);
short get_mem16(unsigned short addr);
void set_mem8(unsigned short addr, char value);
void set_mem16(unsigned short addr, short vale);

char *mem_base;

#define INTERRUPT_VECTOR    0x0
#define V_BLANK_INT         0x40
#define LCD_STAT_INT        0x48
#define TIMER_INT           0x50
#define SERIAL_INT          0x58
#define JOYPAD_INT          0x60
#define CARTRIDGE_HEADER    0x100
#define ROM_BANK_0          0x150
#define ROM_BANK_N          0x4000
#define CHARACTER_RAM       0x8000
#define BG_MAP_DATA_1       0x9800
#define BG_MAP_DATA_2       0x9c00
#define EXTERNAL_RAM        0xa000
#define INTERNAL_RAM        0xc000
#define RESERVED_AREA       0xe000
#define OBJECT_ATTRIBUTE    0xfe00
#define UNUSED              0xfea0
#define HARDWARE_IO_REGS    0xff00
#define INTERRUPT_FLAG      0xff0f
#define HIGH_RAM_AREA       0xff80
#define INTERRUPT_ENABLE    0xffff
