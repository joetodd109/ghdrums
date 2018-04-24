SRCS = main.c iox.c uart.c timer.c utl.c midi.c system_stm32f4xx.c

PROJ_NAME=midi

###################################################

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
LD=arm-none-eabi-ld
AR=arm-none-eabi-ar

CFLAGS  = -g -O2 -Wall -Tstm32_flash.ld --specs=nosys.specs
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=softfp -mfpu=fpv4-sp-d16

###################################################

vpath %.c src

ROOT=$(shell pwd)

CFLAGS += -Iinc
CFLAGS += -Iinc/core

SRCS += src/startup_stm32f4xx.s

OBJS = $(SRCS:.c=.o)

###################################################

.PHONY: proj

all: proj

proj: 	$(PROJ_NAME).elf

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

flash:
	openocd -f board/stm32f4discovery.cfg -c \
		"init; reset halt; flash write_image erase midi.bin 0x08000000; reset run; shutdown"

clean:
	rm -f *.o
	rm -f $(PROJ_NAME).elf
	rm -f $(PROJ_NAME).hex
	rm -f $(PROJ_NAME).bin
