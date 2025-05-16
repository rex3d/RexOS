#include "multitasking.h"
#include "video.h"

extern void switch_task64_asm(uint64_t* old_rsp, uint64_t* old_rbp, uint64_t new_rsp, uint64_t new_rbp);

uint8_t stack_64[MAX_TASKS][STACK_SIZE_64];
task_t tasks64[MAX_TASKS];
int current_task64 = 0;

void multitasking_init64() {
    video_print("Multitasking 64-bit init\n");
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks64[i].arch = ARCH_64BIT;
        tasks64[i].t64.rsp = (uint64_t)&stack_64[i][STACK_SIZE_64 - 8];
        tasks64[i].t64.rbp = tasks64[i].t64.rsp;
    }
}

void switch_task64() {
    int prev = current_task64;
    current_task64 = (current_task64 + 1) % MAX_TASKS;

    switch_task64_asm(&tasks64[prev].t64.rsp, &tasks64[prev].t64.rbp,
                     tasks64[current_task64].t64.rsp, tasks64[current_task64].t64.rbp);
}
