void init_interrupt(void);
void check_interrupts(void);

char ime;

#define TEST_IF(flag)  ((get_mem8(INTERRUPT_FLAG) & flag##_FLAG) >> flag##_SHIFT)
#define SET_IF(flag)   set_mem8(INTERRUPT_FLAG, (get_mem8(INTERRUPT_FLAG) | flag##_FLAG))
#define CLEAR_IF(flag) set_mem8(INTERRUPT_FLAG, (get_mem8(INTERRUPT_FLAG) & ~flag##_FLAG))

#define TEST_IE(flag)  ((get_mem8(INTERRUPT_ENABLE) & flag##_FLAG) >> flag##_SHIFT)
#define SET_IE(flag)   set_mem8(INTERRUPT_ENABLE, (get_mem8(INTERRUPT_ENABLE) | flag##_FLAG))
#define CLEAR_IE(flag) set_mem8(INTERRUPT_ENABLE, (get_mem8(INTERRUPT_ENABLE) & ~flag##_FLAG))

#define INTERRUPT(flag) SET_IF(flag)

/* Flag bitmasks for interrupt parsing */
#define V_BLANK_FLAG   0x1
#define LCD_STAT_FLAG  0x2
#define TIMER_FLAG     0x4
#define SERIAL_FLAG    0x8
#define JOYPAD_FLAG    0x10
#define V_BLANK_SHIFT  0
#define LCD_STAT_SHIFT 1
#define TIMER_SHIFT    2
#define SERIAL_SHIFT   3
#define JOYPAD_SHIFT   4
