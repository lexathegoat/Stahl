// memory.c - Basit heap bellek yöneticisi
#include "headers.h"

static mem_block_t* heap_start = NULL;
static uint32_t total_memory = HEAP_SIZE;
static uint32_t used_memory = 0;

void init_memory() {
    heap_start = (mem_block_t*)HEAP_START;
    heap_start->size = HEAP_SIZE - sizeof(mem_block_t);
    heap_start->is_free = 1;
    heap_start->next = NULL;
    used_memory = sizeof(mem_block_t);
}

void* kmalloc(uint32_t size) {
    if(size == 0)
        return NULL;

    // 4-byte hizalama
    size = (size + 3) & ~3;

    mem_block_t* current = heap_start;

    // First-fit algoritması
    while(current != NULL) {
        if(current->is_free && current->size >= size) {
            // Blok bulundu
            if(current->size > size + sizeof(mem_block_t) + 16) {
                // Bloğu böl
                mem_block_t* new_block = (mem_block_t*)((uint8_t*)current + sizeof(mem_block_t) + size);
                new_block->size = current->size - size - sizeof(mem_block_t);
                new_block->is_free = 1;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }

            current->is_free = 0;
            used_memory += size + sizeof(mem_block_t);

            return (void*)((uint8_t*)current + sizeof(mem_block_t));
        }
        current = current->next;
    }

    return NULL; // Bellek yetersiz
}

void kfree(void* ptr) {
    if(ptr == NULL)
        return;

    mem_block_t* block = (mem_block_t*)((uint8_t*)ptr - sizeof(mem_block_t));
    block->is_free = 1;
    used_memory -= block->size + sizeof(mem_block_t);

    // Bitişik boş blokları birleştir
    mem_block_t* current = heap_start;
    while(current != NULL && current->next != NULL) {
        if(current->is_free && current->next->is_free) {
            current->size += sizeof(mem_block_t) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

uint32_t get_total_memory() {
    return total_memory;
}

uint32_t get_used_memory() {
    return used_memory;
}

void* memset(void* dest, int val, size_t len) {
    uint8_t* d = (uint8_t*)dest;
    while(len--) {
        *d++ = (uint8_t)val;
    }
    return dest;
}

void* memcpy(void* dest, const void* src, size_t len) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    while(len--) {
        *d++ = *s++;
    }
    return dest;
}