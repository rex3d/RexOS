#include "multitasking.h"
#include "video.h"

task_t tasks[MAX_TASKS];
int current_task = 0;

uint8_t stack_16[MAX_TASKS][STACK_SIZE_16];
uint8_t stack_32[MAX_TASKS][STACK_SIZE_32];

extern void switch_task16_asm(uint16_t* old_sp, uint16_t* old_bp, uint16_t new_sp, uint16_t new_bp);
extern void switch_task32_asm(uint32_t* old_esp, uint32_t* old_ebp, uint32_t new_esp, uint32_t new_ebp);

void multitasking_init() {
    video_print("Multitasking 16/32 init\n");
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].arch = ARCH_32BIT; // domyślnie 32-bit, możesz zmienić
        tasks[i].t32.esp = (uint32_t)&stack_32[i][STACK_SIZE_32 - 4];
        tasks[i].t32.ebp = tasks[i].t32.esp;
    }
}

void switch_task() {
    int prev = current_task;
    current_task = (current_task + 1) % MAX_TASKS;

    if (tasks[prev].arch == ARCH_16BIT) {
        switch_task16_asm(&tasks[prev].t16.sp, &tasks[prev].t16.bp,
                          tasks[current_task].t16.sp, tasks[current_task].t16.bp);
    } else if (tasks[prev].arch == ARCH_32BIT) {
        switch_task32_asm(&tasks[prev].t32.esp, &tasks[prev].t32.ebp,
                          tasks[current_task].t32.esp, tasks[current_task].t32.ebp);
    }
}
