GCC:=riscv-none-elf-gcc
OBJCOPY:=riscv-none-elf-objcopy
ASMDIR:=asm/
SRCDIR:=src/
MAINDIR:=main/
TINYMAINDIR:=tiny_main/
ODIR=obj/
TINYODIR=tiny_obj/
OFILES := $(patsubst %.S,%.S.o,$(wildcard $(ASMDIR)*.S))
OFILES += $(patsubst %.c,%.c.o,$(wildcard $(SRCDIR)*.c))
OFILES += $(patsubst %.c,%.c.o,$(wildcard $(SRCDIR)*.c))
TINYOFILES := $(OFILES)
OFILES += $(patsubst %.c,%.c.o,$(wildcard $(MAINDIR)*.c))
OFILES := $(patsubst $(ASMDIR)%,$(ODIR)%,$(OFILES))
OFILES := $(patsubst $(SRCDIR)%,$(ODIR)%,$(OFILES))
OFILES := $(patsubst $(MAINDIR)%,$(ODIR)%,$(OFILES))
TINYOFILES += $(patsubst %.c,%.c.o,$(wildcard $(TINYMAINDIR)*.c))
TINYOFILES := $(patsubst $(ASMDIR)%,$(TINYODIR)%,$(TINYOFILES))
TINYOFILES := $(patsubst $(SRCDIR)%,$(TINYODIR)%,$(TINYOFILES))
TINYOFILES := $(patsubst $(TINYMAINDIR)%,$(TINYODIR)%,$(TINYOFILES))
TARGET:=final
TARGETBIN:=final.bin
TINYTARGET:=final_tiny
TINYTARGETBIN:=final_tiny.bin
TINYTARGETTXT:=final_tiny.txt
LDFLAGS := -Wl,--gc-sections -nodefaultlibs -nostartfiles -lc -lgcc
CFLAGS := -Wall -Wextra -MMD -MP --std=gnu23 -Iinc -mbranch-cost=3
ARCHFLAGS := -march=rv32im -mabi=ilp32 -mlittle-endian
.PHONY: all clean

-include $(OFILES:%.o=%.d)
-include $(TINYOFILES:%.o=%.d)

.DEFAULT_GOAL := all

all: $(TARGETBIN) $(TINYTARGETTXT)

$(ODIR) $(TINYODIR):
	mkdir -p $(@)

$(ODIR)%.S.o: $(ASMDIR)%.S | $(ODIR)
	$(GCC) $(ARCHFLAGS) -c $< -o $@

$(ODIR)%.c.o: $(SRCDIR)%.c | $(ODIR)
	$(GCC) $(CFLAGS) $(ARCHFLAGS) -O3 -c $< -o $@

$(ODIR)%.c.o: $(MAINDIR)%.c | $(ODIR)
	$(GCC) $(CFLAGS) $(ARCHFLAGS) -O3 -c $< -o $@

$(TARGET): $(OFILES)
	$(GCC) -o $@ $^ $(LDFLAGS) -Tlinker_script.ld $(ARCHFLAGS)

$(TARGETBIN): $(TARGET)
	$(OBJCOPY) -O binary $^ $@

$(TARGETTXT): $(TARGETBIN)
	od --address-radix=n --output-duplicates --width=4 --format=x4 $^ | tr -d ' ' > $@

$(TINYODIR)%.S.o: $(ASMDIR)%.S | $(TINYODIR)
	$(GCC) $(ARCHFLAGS) -D_TINY_BUILD_ -c $< -o $@

$(TINYODIR)%.c.o: $(SRCDIR)%.c | $(TINYODIR)
	$(GCC) $(CFLAGS) -ffunction-sections $(ARCHFLAGS) -D_TINY_BUILD_ -Os -c $< -o $@

$(TINYODIR)%.c.o: $(TINYMAINDIR)%.c | $(TINYODIR)
	$(GCC) $(CFLAGS) $(ARCHFLAGS) -D_TINY_BUILD_ -Os -c $< -o $@

$(TINYTARGET): $(TINYOFILES)
	$(GCC) -o $@ $^ $(LDFLAGS) -Tlinker_script.ld $(ARCHFLAGS)

$(TINYTARGETBIN): $(TINYTARGET)
	$(OBJCOPY) -O binary $^ $@

$(TINYTARGETTXT): $(TINYTARGETBIN)
	od --address-radix=n --output-duplicates --width=4 --format=x4 $^ | tr -d ' ' > $@

clean:
	rm -rf $(ODIR) $(TINYODIR) $(TARGET) $(TARGETBIN) $(TINYTARGET) $(TINYTARGETBIN) $(TINYTARGETTXT)
