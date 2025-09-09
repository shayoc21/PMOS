rm -rf build/*
mkdir -p build/bootloader
nasm src/bootloader/boot.asm -f bin -o build/bootloader/bootloader.bin
nasm src/bootloader/stagetwo.asm -f bin -o build/bootloader/stwo.bin
dd if=/dev/zero of=build/bootloader/boot_floppy.img bs=512 count=2880
mkfs.fat -F 12 -n "BOOT" build/bootloader/boot_floppy.img
dd if=build/bootloader/bootloader.bin of=build/bootloader/boot_floppy.img conv=notrunc 
mcopy -i build/bootloader/boot_floppy.img build/bootloader/stwo.bin "::stwo.bin"
