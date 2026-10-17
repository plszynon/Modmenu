# Makefile pod PSL1GHT (ps3dev). NIEZWERYFIKOWANY - może wymagać poprawek.
ifeq ($(strip $(PS3DEV)),)
$(error Ustaw PS3DEV)
endif
include $(PSL1GHT)/ppu_rules

TARGET  := mcmenu
SOURCES := mcmenu.c
CFLAGS  := -O2 -Wall -mcpu=cell
LDFLAGS := -L$(PSL1GHT)/ppu/lib
LIBS    := -lio -lsysutil -lsysmodule -lrt -llv2

all: $(TARGET).elf

$(TARGET).elf: $(SOURCES) offsets.h
	$(PPU_CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) $(LIBS) -o $@

clean:
	rm -f $(TARGET).elf
