// kernel.h - Tüm header tanımlamaları
#ifndef KERNEL_H
#define KERNEL_H

// ============================================
// Temel Tip Tanımları
// ============================================
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

typedef unsigned int size_t;
#define NULL ((void*)0)

// ============================================
// Port I/O
// ============================================
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);

// ============================================
// String Fonksiyonları
// ============================================
int strlen(const char* str);
int strcmp(const char* s1, const char* s2);
void* memset(void* dest, int val, size_t len);
void* memcpy(void* dest, const void* src, size_t len);

// ============================================
// Matematik
// ============================================
float sin(float x);
float cos(float x);
int rand();

// ============================================
// Screen (VGA)
// ============================================
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

enum vga_color {
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15
};

void init_screen();
void clear_screen();
void kprint(const char* str);
void kprint_char(char c);
void kprint_dec(uint32_t n);
void kprint_hex(uint32_t n);
void kprint_backspace();
void set_color(uint8_t fg, uint8_t bg);
void plot_pixel(int x, int y, uint8_t color);

// ============================================
// Keyboard
// ============================================
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

void init_keyboard();
char get_key();
void keyboard_handler();

// ============================================
// Memory Management
// ============================================
#define HEAP_START 0x100000
#define HEAP_SIZE 0x100000
#define BLOCK_SIZE 4096

typedef struct mem_block {
    uint32_t size;
    uint8_t is_free;
    struct mem_block* next;
} mem_block_t;

void init_memory();
void* kmalloc(uint32_t size);
void kfree(void* ptr);
uint32_t get_total_memory();
uint32_t get_used_memory();

// ============================================
// Task Scheduler
// ============================================
#define MAX_TASKS 32

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_TERMINATED
} task_state_t;

typedef struct {
    uint32_t id;
    char name[32];
    task_state_t state;
    uint32_t esp;
    uint32_t ebp;
    uint32_t eip;
    uint32_t priority;
} task_t;

void init_scheduler();
void create_task(void (*entry)(), const char* name, uint32_t priority);
void schedule();
void list_tasks();
task_t* get_current_task();

#endif