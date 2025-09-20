;
;	Stage 1 bootloader..
;	Lives at 0x00 on the disk, loaded into 0x7C00 in memory
;	Defines BPB header
;	Loads stage 2 from LBA 1 using extended read
;	

org 0x7C00
bits 16

	; BIOS Parameter Block							Offsets, for addressing later
	jmp short start								;0x7C00
	nop
	bpb_oem 		db 'mkdosfs '					;0x7C03
	bpb_bytes_per_sector	dw 0x0200					;0x7C0B
	bpb_sectors_per_cluster db 0x08						;0x7C0D
	bpb_reserved_sectors	dw 0x0020					;0x7C0E
	bpb_fat_count		db 2						;0x7C10
	bpb_directory_entries	dw 0x0000	; irrelevant			;0x7C11
	bpb_logical_sectors	dw 0x0000	; > 65535			;0x7C13
	bpb_media_type		db 0xF8		; hard disk			;0x7C15
	bpb_sectors_per_fat	dw 0x0000	; irrelevant			;0x7C16
	bpb_sectors_per_track	dw 0x003F					;0x7C18
	bpb_number_of_heads	dw 0x0040					;0x7C1A
	bpb_hidden_sector_count dd 0x00000000					;0x7C1C
	bpb_large_sector_count	dd 0x001FFFF8	; 1GB				;0x7C20
	
	; Extended Boot Record (FAT32)
	ebr_sectors_per_fat	dd 0x00000800					;0x7C24
	ebr_flags		dw 0x0000					;0x7C28
	ebr_fat_version_no	dw 0x0000					;0x7C2A
	ebr_root_cluster_no	dd 0x00000002					;0x7C2C
	ebr_fsinfo_sector_no	dw 0x0001					;0x7C30
	ebr_backup_sector_no	dw 0x0006					;0x7C32
				dd 0x00000000	; 12 bytes reserved		;0x7C34
				dd 0x00000000				
				dd 0x00000000					
	ebr_drive_no		db 0x80		; 0x80 for hard disk		;0x7C40
				db 0x00		; NT flags, reserved 		;0x7C41
				db 0x29		; signature			;0x7C42
				dd 0x00000000	; Volume ID, reserved/ignored	;0x7C43
				db 'PMOS       '; Volume Label			;0x7C47
				db 'FAT32   '	; Identifier			;0x7C52

start:
	jmp main

%include "./src/bootloader/include/print.asm"

main:
	cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7C00
	sti

	pusha
	mov ax, 0x0003
	int 0x10
	popa
	
	mov si, loading
	call print
	
	mov dl, byte [ebr_drive_no]	
	mov si, DAP
	mov ax, 0x4200	; extended read, lacks compatibility with chs systems
	int 0x13
	jc disk_error
	mov si, success
	call print

	push 0x07E0
	push 0x0000
	retf		; jumps into stage two

disk_error:
	mov si, error
	call print
.halt:
	jmp .halt

strings:
	loading db "[*] Loading Stage Two..", 0x0D, 0x0A, 0x00
	success db "[+] Success..", 0x0D, 0x0A, 0x00
	error 	db "[-] Error..", 0x0D, 0x0A, 0x00  

times 510-16-($-$$) db 0x00
DAP:			; Disk Access Packet
	db 0x10
	db 0x00
	dw 0x0016	; number of sectors to read
	dw 0x0000	; offset to read into
	dw 0x07E0	; segment to read into
	dd 0x00000002
	dd 0x00000000

; Magic number 0xAA55 indicates this sector contains valid boot code.
			dw 0xAA55
