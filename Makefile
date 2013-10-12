all: cardamine

EXTRA_CFLAGS = -Wall -g

cardamine: main.o cpu.o mem.o rom.o io_regs.o joypad.o timer.o interrupt.o audio.o video.o serial.o
	$(CC) main.o cpu.o mem.o rom.o io_regs.o joypad.o timer.o interrupt.o audio.o video.o serial.o -o cardamine

main.o: main.c
	$(CC) -c main.c $(EXTRA_CFLAGS)

cpu.o: cpu.c
	$(CC) -c cpu.c $(EXTRA_CFLAGS)

mem.o: mem.c
	$(CC) -c mem.c $(EXTRA_CFLAGS)

rom.o: rom.c
	$(CC) -c rom.c $(EXTRA_CFLAGS)

io_regs.o: io_regs.c
	$(CC) -c io_regs.c $(EXTRA_CFLAGS)

joypad.o: joypad.c
	$(CC) -c joypad.c $(EXTRA_CFLAGS)

timer.o: timer.c
	$(CC) -c timer.c $(EXTRA_CFLAGS)

interrupt.o: interrupt.c
	$(CC) -c interrupt.c $(EXTRA_CFLAGS)

audio.o: audio.c
	$(CC) -c audio.c $(EXTRA_CFLAGS)

video.o: video.c
	$(CC) -c video.c $(EXTRA_CFLAGS)

serial.o: serial.c
	$(CC) -c serial.c $(EXTRA_CFLAGS)

clean:
	rm -rf *.o cardamine

