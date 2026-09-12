ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment")
endif

DEVKITA64 ?= $(DEVKITPRO)/devkitA64
PREFIX    := $(DEVKITA64)/bin/aarch64-none-elf-
AS        := $(PREFIX)as
LD        := $(PREFIX)ld
OBJCOPY   := $(PREFIX)objcopy
OBJDUMP   := $(PREFIX)objdump

BUILD     := build
TARGET    := $(BUILD)/cpu_nzcv_cinc_001

ASFLAGS   := -march=armv8-a -I ../include
LDFLAGS   := -T linker.ld --build-id=none

.PHONY: all clean
all: $(TARGET).bin $(TARGET).disasm.txt

$(BUILD):
	@mkdir -p $@

$(TARGET).o: cpu_nzcv_cinc_001.s ../include/cpu_nzcv_cinc_core.inc | $(BUILD)
	$(AS) $(ASFLAGS) -o $@ $<

$(TARGET).elf: $(TARGET).o linker.ld
	$(LD) $(LDFLAGS) -o $@ $<

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(TARGET).disasm.txt: $(TARGET).elf
	$(OBJDUMP) -d $< > $@

clean:
	rm -rf $(BUILD)
