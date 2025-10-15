[BITS 16]
[ORG 0x7C00]

start:
    cli                     ; Interrupt'ları kapat
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00          ; Stack pointer ayarla
    sti                     ; Interrupt'ları aç

    ; Ekranı temizle
    mov ah, 0x00
    mov al, 0x03
    int 0x10

    ; "DivineOS" başlık göster
    mov si, msg_boot
    call print_string

    ; Disk'ten kernel'ı yükle (1. sektörden sonra)
    mov ah, 0x02            ; Read sectors
    mov al, 15              ; 15 sektör oku (kernel için)
    mov ch, 0               ; Cylinder 0
    mov cl, 2               ; Sector 2'den başla
    mov dh, 0               ; Head 0
    mov bx, 0x1000          ; ES:BX = 0x0000:0x1000
    int 0x13
    jc disk_error

    ; Protected mode'a geç
    call enable_a20
    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:protected_mode

disk_error:
    mov si, msg_disk_error
    call print_string
    jmp $

enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret

msg_boot db 'DivineOS v1.0 Booting...', 0x0D, 0x0A, 0
msg_disk_error db 'Disk Read Error!', 0x0D, 0x0A, 0

; GDT (Global Descriptor Table)
gdt_start:
    dq 0x0000000000000000   ; Null descriptor

gdt_code:
    dw 0xFFFF               ; Limit
    dw 0x0000               ; Base (low)
    db 0x00                 ; Base (middle)
    db 10011010b            ; Access byte
    db 11001111b            ; Flags + Limit (high)
    db 0x00                 ; Base (high)

gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[BITS 32]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    ; Kernel'a atla
    jmp 0x1000

times 510-($-$$) db 0
dw 0xAA55