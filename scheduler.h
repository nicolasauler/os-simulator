#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdint.h>

#define MAXPRCS 64

/*
        p[i].pid = -1;
        p[i].mem_size = 0;
        p[i].mem_start = 0;
        p[i].mem_end = 0;
        p[i].mem_used = 0;
        p[i].mem_free = 0;
        p[i].mem = NULL;
        p[i].state = 0;
        p[i].priority = 0;
        p[i].time_quantum = 0;
        p[i].time_remaining = 0;
        p[i].time_used = 0;
        p[i].time_waiting = 0;
        p[i].time_turnaround = 0;
        p[i].time_response = 0;
        */

typedef struct process {
    uint8_t pid;
    int mem_size;
    int mem_start;
    int mem_end;
    int mem_used;
    int mem_free;
    uint8_t *mem;
    uint8_t state;
    uint8_t priority;
    int time_quantum;
    int time_remaining;
    int time_used;
    int time_waiting;
} process;

enum process_state {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
};

void create_process(unsigned int pid, int mem_size,
                    process process_list[MAXPRCS]);
void kill_process(uint8_t pid, process process_list[MAXPRCS]);

#endif
