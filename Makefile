PROJECT=stm32tmpl

CPU=cortex-m4
SRCDIR=src/
INCDIR=inc/
BUILDDIR=build/
OUTDIR=out/

SOURCES=$(wildcard $(SRCDIR)*.c)
#INCLUDES=$(addprefix -I, $(sort $(dir $(wildcard $(SRCDIR)*/))))
INCLUDES=libll libcmsis libcmsis/core

LINKSCRIPT=stm32l432.ld
CFLAGS=-O0 -Wall -c -mcpu=$(CPU) -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 $(addprefix -I$(SRCDIR), $(INCLUDES))
TGT=$(OUTDIR)$(PROJECT)
CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
OBJCOPY=arm-none-eabi-objcopy

RM=rm -rf

.PHONY: build
build: $(BUILDDIR) $(OUTDIR) $(TGT).elf $(TGT).bin

.PHONY: clean
clean: 
	$(RM) $(BUILDDIR) $(OUTDIR)

$(TGT).elf: $(patsubst $(SRCDIR)%.c, $(BUILDDIR)%.o, $(SOURCES))
	$(LD) -T $(LINKSCRIPT) -o $@ $^

$(TGT).bin: $(TGT).elf
	$(OBJCOPY) $< $@ -O binary

$(BUILDDIR)%.o:: $(SRCDIR)%.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILDDIR):
	mkdir $@

$(OUTDIR):
	mkdir $@
