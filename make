rm -rf build/*
mkdir -p build/bootloader
nasm src/bootloader/boot.asm -f bin -o build/bootloader/bootloader.bin
nasm src/bootloader/stagetwo.asm -f bin -o build/bootloader/stwo.bin
qemu-img create -f raw build/hdd.img 64M
dd if=/dev/zero of=build/hdd.img bs=1M count=64 conv=notrunc
mkfs.fat -F 32 -n "PMOS" build/hdd.img
dd if=build/bootloader/bootloader.bin of=build/hdd.img bs=512 seek=0 conv=notrunc
dd if=build/bootloader/stwo.bin of=build/hdd.img bs=512 seek=2 conv=notrunc

