#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "common.h"
#include "io_regs.h"
#include "mem.h"
#include "rom.h"
#include "cpu.h"
#include "audio.h"
#include "video.h"
#include "interrupt.h"
#include "timer.h"
#include "joypad.h"
#include "serial.h"

#define PAGE_SIZE getpagesize()

char *mem_base;

int main ( int argc, char **argv )
{
    init_mem();
    init_rom(argv[1]);
    init_cpu();
    init_interrupt();
    init_audio();
    init_video();
    init_timer();
    init_joypad();
    init_serial();

    /* Main loop */
    while ( 1 )
    {
        exec_instruction();
        check_interrupts();
        cycle_video();
        cycle_timer();
    }

    return 0;
}
