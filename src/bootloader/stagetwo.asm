;
;	Needs rewrite for readability in the future..
;

org 0x7E00
bits 16

start:
	jmp main


;empty DAP to use for extended reads

times 16-($-$$) db 0 ; align the DAP to 16 byte boundary

DAP:
	db 0x10
	db 0x00
.toread	dw 0x0000
.offset	dw 0x0000
.segment dw 0x0000
.lba	dq 0x0000000000000000

clusterhigh dw 0x0000
clusterlow  dw 0x0000

%include "./include/print.asm"

CODESEG equ csdescriptor - gdtstart
DATASEG equ dsdescriptor - gdtstart

message 	db "[+] Stage Two Loaded..", 0x0D, 0x0A, 0x00
a20message	db "[+] A20 Line Enabled..", 0x0D, 0X0A, 0X00
protected	db "[*] Entering Protected Mode..", 0x0D, 0x0A, 0x00

main:
	mov ax, 0x00
	mov ds, ax
	mov es, ax

	mov si, message
	call print

a20:
	; some emulators, like qemu, enable the a20 line by default
	; therefore I will test it, by comparing 0x07C0:01FE (0xAA55) to 0xFFFF:7E0E
	mov ax, 0xFFFF
	mov es, ax
	mov ax, ds:[0x7C00]
	mov bx, es:[0x7E0E]
	cmp ax, bx
	jz .seta20
	; else a20 has been initialised
	jmp .a20exit

.seta20:
	in al, 0x92
	or al, 0x02
	out 0x92, al
	
.a20exit:
	mov si, a20message
	call print

.pause: 				
	pusha

	mov si, protected
	call print
	
	;bios stores an 18.2Hz tick counter at 0x0040:0x006C
	push es
	mov ax, 0x0040
	mov es, ax
	mov bx, [es:0x006C]
	mov cx, bx
	add cx, 16
	;wait routine will rest the pc for a second... completely cosmetic feature but makes boot more 'realistic'
.wait:
	mov ax, [es:0x006C]
	cmp ax, cx
	jb .wait
	pop es
	popa


.loadprotectedmode:
	cli
	lgdt [gdtdescriptor]
	mov eax, cr0
	or eax, 1
	mov cr0, eax 	; in 32 bit protected mode now

	; when handling GDT selectors, bits 3-15 are the index of the descriptor, since tpl and rpl = 0, the byte offset = the correct selector implicitly
	jmp CODESEG:0x8200	; flat binary loader.bin is linked to start at 0x8200, it will handle the protected mode loading of the kernel

GDT:
	
gdtstart:
	dq 0x0
csdescriptor:
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 0b10011010
	db 0b11001111
	db 0x0
dsdescriptor:
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 0b10010010
	db 0b11001111
	db 0x0
gdtend:
	
gdtdescriptor:
	dw gdtend - gdtstart - 1
	dd gdtstart


