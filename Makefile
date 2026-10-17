.RECIPEPREFIX = >
TARGET  := mcmenu
CC      := ppu-gcc
CFLAGS  := -O2 -Wall -mcpu=cell -I$(PSL1GHT)/ppu/include -I$(PS3DEV)/ppu/include
LDFLAGS := -L$(PSL1GHT)/ppu/lib -L$(PS3DEV)/ppu/lib
LIBS    := -lio -lsysutil -lsysmodule -lrt -llv2

all: $(TARGET).elf

$(TARGET).elf: mcmenu.c offsets.h
> $(CC) $(CFLAGS) mcmenu.c $(LDFLAGS) $(LIBS) -o $@

clean:
> rm -f $(TARGET).elf
