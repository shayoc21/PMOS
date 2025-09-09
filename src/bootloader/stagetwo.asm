org 0x500

start:
	jmp main


%include "./include/print.asm"

main:
	mov ax, 0x00
	mov ds, ax
	mov es, ax

	mov si, message
	call print


section .data:
	message db "Stage Two Loaded!", 0x0D, 0x0A, 0x00
	
