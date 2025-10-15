# DivineOS Makefile
# Cross-compiler ve linker ayarları
AS = /usr/bin/nasm
CC = gcc
AS = nasm
LD = i686-elf-ld

CFLAGS = -m32 -ffreestanding -fno-builtin -fno-stack-protector \
         -Wall -Wextra -c -nostdinc -I.

LDFLAGS = -m elf_i386 -T linker.ld

ASFLAGS = -f elf32

# Kaynak dosyalar
ASM_SOURCES = boot.asm
C_SOURCES = kernel.c screen.c keyboard.c memory.c task.c

# Obje dosyaları
ASM_OBJECTS = $(ASM_SOURCES:.asm=.o)
C_OBJECTS = $(C_SOURCES:.c=.o)

all: divineos.img

# Bootloader'ı derle
boot.bin: boot.asm
	$(AS) -f bin $< -o $@

# C dosyalarını derle
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# Assembly dosyalarını derle (kernel için)
%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

# Kernel'ı link et
kernel.bin: $(C_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

# Disk image oluştur
divineos.img: boot.bin kernel.bin
	cat boot.bin kernel.bin > divineos.img
	# 1.44MB floppy image'a pad et
	truncate -s 1440K divineos.img

# QEMU ile çalıştır
run: divineos.img
	qemu-system-i386 -fda divineos.img -monitor stdio

# Bochs ile debug
debug: divineos.img
	bochs -f bochsrc.txt -q

# Temizlik
clean:
	rm -f *.o *.bin *.img

# ISO oluştur (GRUB kullanarak)
iso: kernel.bin
	mkdir -p isodir/boot/grub
	cp kernel.bin isodir/boot/kernel.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o divineos.iso isodir
	rm -rf isodir

.PHONY: all run debug clean iso
