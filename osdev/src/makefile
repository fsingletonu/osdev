BUILD:=../build
SRC:=.

ENTERPOINT:=0x10000
VAENTERPOINT:=0xc0000000

CFLAGS:= -m32
CFLAGS+= -fno-builtin
CFLAGS+= -nostdinc
CFLAGS+= -fno-pic
CFLAGS+= -fno-pie
CFLAGS+= -nostdlib
CFLAGS+= -fno-stack-protector
CFLAGS:=$(strip $(CFLAGS))

DEBUG:= -g
INCLUDE:= -I$(SRC)/include

$(BUILD)/boot/%.bin : $(SRC)/boot/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f bin $< -o $@

$(BUILD)/kernel/%.o : $(SRC)/kernel/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@

$(BUILD)/kernel/%.o : $(SRC)/kernel/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@ 

$(BUILD)/kernel.bin : $(BUILD)/kernel/start.o \
	$(BUILD)/kernel/main.o \
	$(BUILD)/kernel/io.o \
	$(BUILD)/kernel/trap.o \
	$(BUILD)/kernel/handler.o \
	$(BUILD)/kernel/interrupt.o \
	$(BUILD)/kernel/console.o \
	$(BUILD)/kernel/head.o \
	$(BUILD)/kernel/memory.o \
	$(BUILD)/kernel/keyboard.o \
	$(BUILD)/kernel/clock.o \
	$(BUILD)/kernel/rtc.o
	$(shell mkdir -p $(dir $@))
	ld -m elf_i386 -static $^ -o $@ -T kernel.ld

$(BUILD)/system.bin : $(BUILD)/kernel.bin
	objcopy -O binary $< $@

$(BUILD)/system.map : $(BUILD)/kernel.bin
	nm $< | sort > $@

$(BUILD)/master.img : $(BUILD)/boot/boot.bin \
	$(BUILD)/boot/loader.bin \
	$(BUILD)/system.bin \
	$(BUILD)/system.map
	yes | bximage -q -hd=16 -mode=create -sectsize=512 -imgmode=flat $@
	dd if=$(BUILD)/boot/boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD)/boot/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
	dd if=$(BUILD)/system.bin of=$@ bs=512 count=240 seek=10 conv=notrunc

test: $(BUILD)/master.img

.PHONY: clean
clean :
	rm -rf $(BUILD)
	rm -rf *.bin
	rm -rf *.img

.PHONY: bochs
bochs : $(BUILD)/master.img
	bochs -q

.PHONY: qemu
qemu : $(BUILD)/master.img
	qemu-system-i386 -m 32M -boot c -hda $<

.PHONY: qemu-gdb
qemu-gdb : $(BUILD)/master.img
	qemu-system-i386 -s -S -m 32M -boot c -hda $<

.PHONY: run
run : $(BUILD)/master.img
	make clean
	make test
	qemu-system-i386 -vga std -d cpu_reset,int,guest_errors -D qemu.log -s -S -m 32M -boot c -hda $<