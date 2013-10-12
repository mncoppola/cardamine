#include <sys/mman.h>
#include "common.h"
#include "rom.h"
#include "io_regs.h"

char *mem_base;

char get_mem8 ( unsigned short addr )
{
    char tmp;

    if ( handle_ioregs_read(addr, &tmp) )
        return tmp;
    else
        return *(mem_base + addr);
}

short get_mem16 ( unsigned short addr )
{
    return *(short *)(mem_base + addr);
}

void set_mem8 ( unsigned short addr, char value )
{
    if ( handle_ioregs_write(addr, value) )
        return;
    else
        *(mem_base + addr) = value;
}

void set_mem16 ( unsigned short addr, short value )
{
    *(short *)(mem_base + addr) = value;
}

void init_mem ( void )
{
    mem_base = (char *)mmap(NULL, 0x10000, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
    if ( mem_base == MAP_FAILED )
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
}
