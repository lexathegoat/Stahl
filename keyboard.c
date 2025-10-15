#include "headers.h"

// US QWERTY klavye layout
static char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8,
    9, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 10,
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 39, 96,
    0, 92, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    42, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static char keyboard_map_shift[128] = {
    0,  27, 33, 64, 35, 36, 37, 94, 38, 42, 40, 41, 95, 43, 8,
    9, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 123, 125, 10,
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 58, 34, 126,
    0, 124, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', 60, 62, 63, 0,
    42, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int shift_pressed = 0;
static int ctrl_pressed = 0;
static int alt_pressed = 0;
static char key_buffer[256];
static int buffer_head = 0;
static int buffer_tail = 0;

void init_keyboard() {
    buffer_head = 0;
    buffer_tail = 0;
}

void keyboard_handler() {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    if(scancode & 0x80) {
        scancode &= 0x7F;
        if(scancode == 0x2A || scancode == 0x36)
            shift_pressed = 0;
        else if(scancode == 0x1D)
            ctrl_pressed = 0;
        else if(scancode == 0x38)
            alt_pressed = 0;
    } else {
        if(scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 1;
        } else if(scancode == 0x1D) {
            ctrl_pressed = 1;
        } else if(scancode == 0x38) {
            alt_pressed = 1;
        } else {
            char key;
            if(shift_pressed)
                key = keyboard_map_shift[scancode];
            else
                key = keyboard_map[scancode];

            if(key != 0) {
                key_buffer[buffer_head] = key;
                buffer_head = (buffer_head + 1) % 256;
            }
        }
    }

    outb(0x20, 0x20);
}

char get_key() {
    while(buffer_head == buffer_tail) {
        asm volatile("hlt");
    }

    char key = key_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % 256;
    return key;
}

int key_available() {
    return buffer_head != buffer_tail;
}
