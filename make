rm -rf build/*
mkdir -p build/bootloader
nasm src/bootloader/boot.asm -f bin -o build/bootloader/bootloader.bin
nasm src/bootloader/stagetwo.asm -f bin -o build/bootloader/stwo.bin

/usr/opt/cross/bin/i386-elf-gcc -ffreestanding -c src/kernel/include/vga_text.c -o build/text.o -O0

/usr/opt/cross/bin/i386-elf-gcc -ffreestanding -c src/kernel/kernel_head.c -o build/loader.o -O0
/usr/opt/cross/bin/i386-elf-ld -T loaderlinker.ld -o build/loader.elf build/loader.o build/text.o -nostdlib
/usr/opt/cross/bin/i386-elf-objcopy -O binary build/loader.elf build/loader.bin


/usr/opt/cross/bin/i386-elf-gcc -ffreestanding -c src/kernel/kernel.c -o build/kernel.o -O0
/usr/opt/cross/bin/i386-elf-gcc -ffreestanding -c src/kernel/interrupts/handlers.c -o build/handlers.o -O0

nasm -f elf32 src/kernel/interrupts/stubs.asm -o build/stubs.o
/usr/opt/cross/bin/i386-elf-ld -T kernellinker.ld -o build/kernel.elf build/kernel.o build/handlers.o build/stubs.o build/text.o -nostdlib
/usr/opt/cross/bin/i386-elf-objcopy -O binary build/kernel.elf build/kernel.bin

qemu-img create -f raw build/hdd.img 1G
dd if=/dev/zero of=build/hdd.img bs=100M count=10 conv=notrunc
mkfs.fat -F 32 -n "PMOS" build/hdd.img
dd if=build/bootloader/bootloader.bin of=build/hdd.img bs=512 seek=0 conv=notrunc
dd if=build/bootloader/stwo.bin of=build/hdd.img bs=512 seek=2 conv=notrunc
dd if=build/loader.bin of=build/hdd.img bs=512 seek=4 conv=notrunc
mcopy -i build/hdd.img build/kernel.bin "::kernel.bin"

