#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "common.h"
#include "cpu.h"
#include "mem.h"
#include "io_regs.h"

char tmp_storage[256];

char gb_boot_rom[256] =
"\x31\xfe\xff\xaf\x21\xff\x9f\x32\xcb\x7c\x20\xfb\x21\x26\xff\x0e"
"\x11\x3e\x80\x32\xe2\x0c\x3e\xf3\xe2\x32\x3e\x77\x77\x3e\xfc\xe0"
"\x47\x11\x04\x01\x21\x10\x80\x1a\xcd\x95\x00\xcd\x96\x00\x13\x7b"
"\xfe\x34\x20\xf3\x11\xd8\x00\x06\x08\x1a\x13\x22\x23\x05\x20\xf9"
"\x3e\x19\xea\x10\x99\x21\x2f\x99\x0e\x0c\x3d\x28\x08\x32\x0d\x20"
"\xf9\x2e\x0f\x18\xf3\x67\x3e\x64\x57\xe0\x42\x3e\x91\xe0\x40\x04"
"\x1e\x02\x0e\x0c\xf0\x44\xfe\x90\x20\xfa\x0d\x20\xf7\x1d\x20\xf2"
"\x0e\x13\x24\x7c\x1e\x83\xfe\x62\x28\x06\x1e\xc1\xfe\x64\x20\x06"
"\x7b\xe2\x0c\x3e\x87\xe2\xf0\x42\x90\xe0\x42\x15\x20\xd2\x05\x20"
"\x4f\x16\x20\x18\xcb\x4f\x06\x04\xc5\xcb\x11\x17\xc1\xcb\x11\x17"
"\x05\x20\xf5\x22\x23\x22\x23\xc9\xce\xed\x66\x66\xcc\x0d\x00\x0b"
"\x03\x73\x00\x83\x00\x0c\x00\x0d\x00\x08\x11\x1f\x88\x89\x00\x0e"
"\xdc\xcc\x6e\xe6\xdd\xdd\xd9\x99\xbb\xbb\x67\x63\x6e\x0e\xec\xcc"
"\xdd\xdc\x99\x9f\xbb\xb9\x33\x3e\x3c\x42\xb9\xa5\xb9\xa5\x42\x3c"
"\x21\x04\x01\x11\xa8\x00\x1a\x13\xbe\x20\xfe\x23\x7d\xfe\x34\x20"
"\xf5\x06\x19\x78\x86\x23\x05\x20\xfb\x86\x20\xfe\x3e\x01\xe0\x50";

void enable_bootROM ( void )
{
    memcpy(tmp_storage, mem_base,  256);
    memcpy(mem_base, gb_boot_rom, 256);
}

void disable_bootROM ( void )
{
    memcpy(mem_base, tmp_storage, 256);

    /* Initialize CPU and memory state */
    SET_AF(0x10b0);
    SET_BC(0x0013);
    SET_DE(0x00d8);
    SET_HL(0x014d);
    sp = 0xfffe;
    set_mem8(TIMA, 0x00);
    set_mem8(TMA,  0x00);
    set_mem8(TAC,  0x00);
    set_mem8(NR10, 0x80);
    set_mem8(NR11, 0xbf);
    set_mem8(NR12, 0xf3);
    set_mem8(NR14, 0xbf);
    set_mem8(NR21, 0x3f);
    set_mem8(NR22, 0x00);
    set_mem8(NR24, 0xbf);
    set_mem8(NR30, 0x7f);
    set_mem8(NR31, 0xff);
    set_mem8(NR32, 0x9f);
    set_mem8(NR33, 0xbf);
    set_mem8(NR41, 0xff);
    set_mem8(NR42, 0x00);
    set_mem8(NR43, 0x00);
    set_mem8(NR44, 0xbf);
    set_mem8(NR50, 0x77);
    set_mem8(NR51, 0xf3);
    set_mem8(NR52, 0xf1);
    set_mem8(LCDC, 0x91);
    set_mem8(SCY,  0x00);
    set_mem8(SCX,  0x00);
    set_mem8(LYC,  0x00);
    set_mem8(BGP,  0xfc);
    set_mem8(OBP0, 0xff);
    set_mem8(OBP1, 0xff);
    set_mem8(WY,   0x00);
    set_mem8(WX,   0x00);
    set_mem8(INTERRUPT_ENABLE, 0x00);
}

void init_rom ( char *rom_filename )
{
    int fd, rom_size;

    if ( (fd = open(rom_filename, O_RDONLY)) < 0 )
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    rom_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    read(fd, mem_base, ROM_BANK_N);

    enable_bootROM();
    disable_bootROM();
}
