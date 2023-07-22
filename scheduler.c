#include "scheduler.h"

void create_process(unsigned int pid, int mem_size,
                    process process_list[MAXPRCS]) {
    process_list[pid].pid = pid;
    process_list[pid].mem_size = mem_size;
    process_list[pid].mem_start = 0;
    process_list[pid].mem_end = 69;
    process_list[pid].state = READY;
}

void kill_process(uint8_t pid, process process_list[MAXPRCS]) {
    process_list[pid].state = TERMINATED;
}
