;
;	Stage 1 bootloader..
;	Lives at 0x00 on the disk, loaded into 0x7C00 in memory
;	Defines BPB header
;	Loads stage 2 from LBA 1 using extended read
;	

org 0x7C00
bits 16

; BIOS Parameter Block
jmp short start
nop
bpb_oem 		db 'mkdosfs '
bpb_bytes_per_sector	dw 0x0200
bpb_sectors_per_cluster db 1
bpb_reserved_sectors	dw 0x0020
bpb_fat_count		db 2
bpb_directory_entries	dw 0x0000	; irrelevant
bpb_logical_sectors	dw 0x0000	; > 65535
bpb_media_type		db 0xF8		; hard disk
bpb_sectors_per_fat	dw 0x0000	; irrelevant
bpb_sectors_per_track	dw 0x0020
bpb_number_of_heads	dw 0x0008	
bpb_hidden_sector_count dd 0x00000000
bpb_large_sector_count	dd 0x00020000	; 131072 sectors, 64MB

; Extended Boot Record (FAT32)
ebr_sectors_per_fat	dd 0x000003F1
ebr_flags		dw 0x0000
ebr_fat_version_no	dw 0x0000
ebr_root_cluster_no	dd 0x00000002
ebr_fsinfo_sector_no	dw 0x0001
ebr_backup_sector_no	dw 0x0006
			dd 0x00000000	; 12 bytes reserved
			dd 0x00000000
			dd 0x00000000
ebr_drive_no		db 0x80		; 0x80 for hard disk
			db 0x00		; NT flags, reserved 
			db 0x29		; signature
			dd 0x00000000	; Volume ID, reserved/ignored
			db 'PMOS       '; Volume Label
			db 'FAT32   '	; Identifier

start:
	jmp main

%include "./include/print.asm"

main:
	cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7C00
	sti
	
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
	loading db " [*] Loading Stage Two..", 0x0D, 0x0A, 0x00
	success db " [+] Success..", 0x0D, 0x0A, 0x00
	error 	db " [-] Error..", 0x0D, 0x0A, 0x00  

times 510-16-($-$$) db 0x00
DAP:			; Disk Access Packet
	db 0x10
	db 0x00
	dw 0x0001	; number of sectors to read
	dw 0x0000	; offset to read into
	dw 0x07E0	; segment to read into
	dd 0x00000002
	dd 0x00000000

; Magic number 0xAA55 indicates this sector contains valid boot code.
			dw 0xAA55
