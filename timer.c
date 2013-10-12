#include "common.h"
#include "cpu.h"
#include "mem.h"
#include "interrupt.h"

unsigned char div_reg;
unsigned char timer_enabled;
unsigned char timer_input_clock_select;
unsigned short timer_counter;
unsigned char timer_modulo;
unsigned char timer_enabled;
unsigned char timer_input_clock_select;
unsigned int timer_cycles;
unsigned int div_cycles;

void increment_timer ( void )
{
    if ( timer_counter > 0xff )
    {
        INTERRUPT(TIMER);
        timer_counter = timer_modulo & 0xff;
    }
    else
    {
        timer_counter++;
    }
}

void update_TIMA ( void )
{
    if ( (timer_input_clock_select == 0) && (timer_cycles >= 1024) )
    {
        timer_cycles -= 1024;
        increment_timer();
    }
    else if ( (timer_input_clock_select == 1) && (timer_cycles >= 16) )
    {
        timer_cycles -= 16;
        increment_timer();
    }
    else if ( (timer_input_clock_select == 2) && (timer_cycles >= 64) )
    {
        timer_cycles -= 64;
        increment_timer();
    }
    else if ( (timer_input_clock_select == 3) && (timer_cycles >= 256) )
    {
        timer_cycles -= 256;
        increment_timer();
    }
}

void update_DIV ( void )
{
    div_cycles += cpu_cycles;
    if ( div_cycles >= 256 )
    {
        div_cycles -= 256;
        div_reg++;
    }
}

void cycle_timer ( void )
{
    update_TIMA();
    update_DIV();
}

void init_timer ( void )
{
    div_reg = 0;
}
