#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdint.h>

#define MAXPRCS 64

typedef struct process {
    int pid;
    int mem_size;
    int mem_start;
    int mem_end;
    int is_active;
} process;

void create_process(unsigned int pid, int mem_size,
                    process process_list[MAXPRCS]);
void kill_process(uint8_t pid, process process_list[MAXPRCS]);

#endif
