// task.c - Basit cooperative multitasking scheduler
#include "headers.h"

static task_t tasks[MAX_TASKS];
static int task_count = 0;
static int current_task_id = -1;

void init_scheduler() {
    for(int i = 0; i < MAX_TASKS; i++) {
        tasks[i].state = TASK_TERMINATED;
    }
    task_count = 0;
    current_task_id = -1;
}

void create_task(void (*entry)(), const char* name, uint32_t priority) {
    if(task_count >= MAX_TASKS) {
        kprint("Task limit reached!\n");
        return;
    }

    int task_id = task_count++;
    tasks[task_id].id = task_id;
    tasks[task_id].state = TASK_READY;
    tasks[task_id].priority = priority;
    tasks[task_id].eip = (uint32_t)entry;

    // İsmi kopyala
    int i;
    for(i = 0; i < 31 && name[i] != '\0'; i++) {
        tasks[task_id].name[i] = name[i];
    }
    tasks[task_id].name[i] = '\0';

    // Stack ayarla (basit implementasyon)
    uint32_t stack = (uint32_t)kmalloc(4096);
    tasks[task_id].esp = stack + 4096;
    tasks[task_id].ebp = tasks[task_id].esp;
}

void schedule() {
    // Basit round-robin scheduler
    if(task_count == 0)
        return;

    int next_task = (current_task_id + 1) % task_count;

    // Ready durumda bir task bul
    int attempts = 0;
    while(tasks[next_task].state != TASK_READY && attempts < task_count) {
        next_task = (next_task + 1) % task_count;
        attempts++;
    }

    if(tasks[next_task].state == TASK_READY) {
        // Mevcut task'ı kaydet
        if(current_task_id >= 0 && tasks[current_task_id].state == TASK_RUNNING) {
            tasks[current_task_id].state = TASK_READY;
        }

        // Yeni task'a geç
        current_task_id = next_task;
        tasks[current_task_id].state = TASK_RUNNING;

        // Context switch yap (basitleştirilmiş)
        // Gerçek implementasyonda register'ları kaydetmek gerekir
    }
}

void list_tasks() {
    if(task_count == 0) {
        kprint("Hic task yok.\n");
        return;
    }

    kprint("ID  Name                State      Priority\n");
    kprint("--- ------------------- ---------- --------\n");

    for(int i = 0; i < task_count; i++) {
        // ID
        kprint_dec(tasks[i].id);
        kprint("   ");

        // Name
        kprint(tasks[i].name);
        for(int j = strlen(tasks[i].name); j < 20; j++)
            kprint(" ");

        // State
        switch(tasks[i].state) {
            case TASK_READY:
                kprint("READY     ");
                break;
            case TASK_RUNNING:
                kprint("RUNNING   ");
                break;
            case TASK_BLOCKED:
                kprint("BLOCKED   ");
                break;
            case TASK_TERMINATED:
                kprint("TERM      ");
                break;
        }
        kprint(" ");

        // Priority
        kprint_dec(tasks[i].priority);
        kprint("\n");
    }
}

task_t* get_current_task() {
    if(current_task_id >= 0 && current_task_id < task_count) {
        return &tasks[current_task_id];
    }
    return NULL;
}

void yield() {
    schedule();
}

void task_exit() {
    if(current_task_id >= 0) {
        tasks[current_task_id].state = TASK_TERMINATED;
        kfree((void*)(tasks[current_task_id].esp - 4096));
    }
    schedule();
}