#include "common.h"
#include "cpu.h"
#include "mem.h"
#include "interrupt.h"

void check_interrupts ( void )
{
    /* Check if interrupts are disabled */
    if ( ime == 0 )
        return;

    /* Handle any queued interrupts */
    if ( TEST_IF(V_BLANK) && TEST_IE(V_BLANK) )
    {
        CLEAR_IF(V_BLANK);
        ime = 0;
        CALL(V_BLANK_INT);
        cpu_cycles = 16;
    }
    else if ( TEST_IF(LCD_STAT) && TEST_IE(LCD_STAT) )
    {
        CLEAR_IF(LCD_STAT);
        ime = 0;
        CALL(LCD_STAT_INT);
        cpu_cycles = 16;
    }
    else if ( TEST_IF(TIMER) && TEST_IE(TIMER) )
    {
        CLEAR_IF(TIMER);
        ime = 0;
        CALL(TIMER_INT);
        cpu_cycles = 16;
    }
    else if ( TEST_IF(SERIAL) && TEST_IE(SERIAL) )
    {
        CLEAR_IF(SERIAL);
        ime = 0;
        CALL(SERIAL_INT);
        cpu_cycles = 16;
    }
    else if ( TEST_IF(JOYPAD) && TEST_IE(JOYPAD) )
    {
        CLEAR_IF(JOYPAD);
        ime = 0;
        CALL(JOYPAD_INT);
        cpu_cycles = 16;
    }

    total_cpu_cycles += cpu_cycles;
}

void init_interrupt( void )
{
    ime = 1;

    set_mem8(INTERRUPT_FLAG, 0);
    set_mem8(INTERRUPT_ENABLE, (V_BLANK_FLAG | LCD_STAT_FLAG | TIMER_FLAG | SERIAL_FLAG | JOYPAD_FLAG));
}
