// screen.c - VGA metin modu sürücüsü
#include "headers.h"

static uint16_t* video_memory = (uint16_t*)VGA_MEMORY;
static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t current_color = 0x0F; // Beyaz üzerine siyah

void init_screen() {
    clear_screen();
    cursor_x = 0;
    cursor_y = 0;
}

void set_color(uint8_t fg, uint8_t bg) {
    current_color = (bg << 4) | (fg & 0x0F);
}

void clear_screen() {
    for(int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video_memory[i] = ' ' | (current_color << 8);
    }
    cursor_x = 0;
    cursor_y = 0;
}

void scroll() {
    // Tüm satırları yukarı kaydır
    for(int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
        video_memory[i] = video_memory[i + VGA_WIDTH];
    }

    // Son satırı temizle
    for(int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
        video_memory[i] = ' ' | (current_color << 8);
    }

    cursor_y = VGA_HEIGHT - 1;
}

void update_cursor() {
    uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void kprint_char(char c) {
    if(c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if(c == '\t') {
        cursor_x = (cursor_x + 4) & ~(4 - 1);
    } else if(c == '\r') {
        cursor_x = 0;
    } else if(c == '\b') {
        if(cursor_x > 0) {
            cursor_x--;
            video_memory[cursor_y * VGA_WIDTH + cursor_x] = ' ' | (current_color << 8);
        }
    } else {
        video_memory[cursor_y * VGA_WIDTH + cursor_x] = c | (current_color << 8);
        cursor_x++;
    }

    if(cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }

    if(cursor_y >= VGA_HEIGHT) {
        scroll();
    }

    update_cursor();
}

void kprint(const char* str) {
    while(*str) {
        kprint_char(*str++);
    }
}

void kprint_backspace() {
    if(cursor_x > 0) {
        cursor_x--;
        video_memory[cursor_y * VGA_WIDTH + cursor_x] = ' ' | (current_color << 8);
        update_cursor();
    }
}

void kprint_dec(uint32_t n) {
    if(n == 0) {
        kprint_char('0');
        return;
    }

    char buffer[12];
    int i = 0;

    while(n > 0) {
        buffer[i++] = '0' + (n % 10);
        n /= 10;
    }

    while(i > 0) {
        kprint_char(buffer[--i]);
    }
}

void kprint_hex(uint32_t n) {
    kprint("0x");

    char hex_chars[] = "0123456789ABCDEF";
    char buffer[9];
    buffer[8] = '\0';

    for(int i = 7; i >= 0; i--) {
        buffer[i] = hex_chars[n & 0xF];
        n >>= 4;
    }

    kprint(buffer);
}

// VGA Mode 13h (320x200, 256 renk) için piksel çizme
void plot_pixel(int x, int y, uint8_t color) {
    if(x >= 0 && x < 320 && y >= 0 && y < 200) {
        uint8_t* vga = (uint8_t*)0xA0000;
        vga[y * 320 + x] = color;
    }
}

// Mode 13h'ye geçiş
void set_vga_mode_13h() {
    // VGA port'larına yazarak Mode 13h'ye geç
    outb(0x3C2, 0x63);
    outb(0x3D4, 0x11);
    outb(0x3D5, 0x00);

    // Sequencer registers
    outb(0x3C4, 0x00); outb(0x3C5, 0x03);
    outb(0x3C4, 0x01); outb(0x3C5, 0x01);
    outb(0x3C4, 0x02); outb(0x3C5, 0x0F);
    outb(0x3C4, 0x03); outb(0x3C5, 0x00);
    outb(0x3C4, 0x04); outb(0x3C5, 0x0E);

    // CRTC registers
    uint8_t crtc_regs[] = {
        0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
        0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3, 0xFF
    };

    for(int i = 0; i < 25; i++) {
        outb(0x3D4, i);
        outb(0x3D5, crtc_regs[i]);
    }

    // Graphics registers
    outb(0x3CE, 0x00); outb(0x3CF, 0x00);
    outb(0x3CE, 0x01); outb(0x3CF, 0x00);
    outb(0x3CE, 0x02); outb(0x3CF, 0x00);
    outb(0x3CE, 0x03); outb(0x3CF, 0x00);
    outb(0x3CE, 0x04); outb(0x3CF, 0x00);
    outb(0x3CE, 0x05); outb(0x3CF, 0x40);
    outb(0x3CE, 0x06); outb(0x3CF, 0x05);
    outb(0x3CE, 0x07); outb(0x3CF, 0x0F);
    outb(0x3CE, 0x08); outb(0x3CF, 0xFF);

    // Attribute registers
    for(int i = 0; i < 16; i++) {
        inb(0x3DA);
        outb(0x3C0, i);
        outb(0x3C0, i);
    }

    inb(0x3DA);
    outb(0x3C0, 0x10); outb(0x3C0, 0x41);
    outb(0x3C0, 0x11); outb(0x3C0, 0x00);
    outb(0x3C0, 0x12); outb(0x3C0, 0x0F);
    outb(0x3C0, 0x13); outb(0x3C0, 0x00);
    outb(0x3C0, 0x14); outb(0x3C0, 0x00);

    outb(0x3C0, 0x20);
}