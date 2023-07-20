#include "scheduler.h"

void create_process(unsigned int pid, int mem_size, process process_list[10]) {
    process_list[pid].pid = pid;
    process_list[pid].mem_size = mem_size;
    process_list[pid].mem_start = 0;
    process_list[pid].mem_end = 69;
    process_list[pid].is_active = 1;
}

/* void kill_process(int pid) {} */
