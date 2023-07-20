#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

typedef struct process {
    int pid;
    int mem_size;
    int mem_start;
    int mem_end;
    int is_active;
} process;

void create_process(unsigned int pid, int mem_size, process process_list[10]);
/* void kill_process(int pid); */

#endif
