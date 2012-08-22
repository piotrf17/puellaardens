# Puella Ardens
# GirlTech Burning Man Communicator

libs = clock.rel compose_view.rel display.rel inbox_view.rel keys.rel pm.rel radio.rel compose_view.rel music.rel
CC = sdcc
CFLAGS = --no-pack-iram
LFLAGS = --xram-loc 0xF000

all: puellaardens.hex

%.rel : %.c
	$(CC) $(CFLAGS) -c $<

puellaardens.hex: puellaardens.rel $(libs)
	sdcc $(LFLAGS) puellaardens.rel $(libs)
	packihx <puellaardens.ihx >puellaardens.hex

install: puellaardens.hex
	cc-tool -e -w puellaardens.hex
verify: puellaardens.hex
	cc-tool -v puellaardens.hex
clean:
	rm -f *.ihx *.rel *.asm *.lst *.rst *.sym *.lnk *.map *.mem
