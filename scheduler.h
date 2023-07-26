#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdint.h>

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
    uint8_t state;
    int time_quantum;
    int time_remaining;
    int time_used;
    int time_waiting;
} process_t;

enum process_state { NEW, READY, RUNNING, WAITING, TERMINATED };

enum scheduler_algorithm { FIFO, ROUND_ROBIN };

typedef struct p_circ_queue {
    process_t *process;
    struct p_circ_queue *prev;
    struct p_circ_queue *next;
} p_circ_queue_t;

process_t *create_process(int mem_size, uint8_t pid);
p_circ_queue_t *add_process_to_queue(p_circ_queue_t *old_queue,
                                     process_t *process_to_add);
p_circ_queue_t *run_process(p_circ_queue_t *queue);
void kill_process(uint8_t pid, p_circ_queue_t *queue);

#endif
