PROJECT=stm32tmpl

CPU=cortex-m4
SRCDIR=src/
INCDIR=inc/
BUILDDIR=build/
OUTDIR=out/

SOURCES=$(shell find $(SRCDIR) -type f -name "*.c")
SRCSUBDIRS=$(shell find $(SRCDIR) -mindepth 1 -type d | cut -d '/' -f2-)

LINKSCRIPT=stm32l432.ld
CFLAGS=-g -O0 -Wall -c -mcpu=$(CPU) -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -I$(SRCDIR) $(addprefix -I$(SRCDIR), $(SRCSUBDIRS))
TGT=$(OUTDIR)$(PROJECT)
CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
OBJCOPY=arm-none-eabi-objcopy

RM=rm -rf

.PHONY: build
build: dirs $(TGT).elf $(TGT).bin

.PHONY: clean
clean: 
	$(RM) $(BUILDDIR) $(OUTDIR)

$(TGT).elf: $(patsubst $(SRCDIR)%.c, $(BUILDDIR)%.o, $(SOURCES))
	$(LD) -T $(LINKSCRIPT) -o $@ $^

$(TGT).bin: $(TGT).elf
	$(OBJCOPY) $< $@ -O binary

$(BUILDDIR)%.o:: $(SRCDIR)%.c $(dir $(BUILDDIR)%.o)
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: dirs
dirs:
	mkdir $(BUILDDIR) $(OUTDIR) $(addprefix $(BUILDDIR), $(SRCSUBDIRS))

.PHONY: flash
flash: build
	st-flash write $(TGT).bin 0x08000000
