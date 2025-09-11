org 0x500
bits 16

start:
	jmp main


%include "./include/print.asm"

CODESEG equ csdescriptor - gdtstart
DATASEG equ dsdescriptor - gdtstart

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
	add cx, 24
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
	jmp CODESEG:startprotectedmode

message db "Stage Two Loaded!", 0x0D, 0x0A, 0x00
a20message db "A20 Line Enabled", 0x0D, 0X0A, 0X00
protected db "Entering Protected Mode...", 0x0D, 0x0A, 0x00

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

section .text

[bits 32]

startprotectedmode:
	
	mov ax, DATASEG
	mov ds, ax
	mov es, ax	; es will also point to the data segment

	call clearvgatextbuffer

	mov si, welcome
	mov ah, 0b00110100
	call printtest
	hlt

;ds:si points to the string to print, ah the attribute byte
; |Attribute byte			Character
; | 7         6,5,4    3,2,1,0       	8 bit ascii character
; |blinking   bg colour foreground colour

printtest:
	pusha
	cld
	xor edi, edi
.printloop:
	lodsb
	cmp al, 0x00
	jz .endprint
	mov [0xb8000 + edi], ax
	add edi, 0x2
	jmp .printloop
.endprint:
	popa
	ret

	welcome db 'Welcome to protected mode!', 0x00

clearvgatextbuffer:
	pusha
	
	mov edi, 0xB8000
	mov ecx, 80*25
	mov al, 0x20 ;space

	mov ah, 0b00110000
	
	rep stosw

	popa
	ret
