#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdint.h>
#include <stdlib.h>

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

/* enum process_state { NEW, READY, RUNNING, WAITING, TERMINATED }; */
enum process_state { READY, RUNNING, BLOCKED, TERMINATED };

enum scheduler_algorithm { FIFO, RR };

typedef struct sched_info {
    enum scheduler_algorithm algorithm;
    int time_quantum;
} sched_info_t;

typedef struct process {
    uint8_t pid;
    int mem_size;
    int8_t mem_start;
    enum process_state state;
    int time_quantum;
    int time_remaining;
    int time_used;
    int time_waiting;
} process_t;

typedef struct p_queue {
    process_t *process;
    struct p_queue *prev;
    struct p_queue *next;
} p_queue_t;

process_t *create_process(int mem_size, uint8_t pid);
p_queue_t *add_process_to_queue(p_queue_t *old_queue,
                                process_t *process_to_add);
p_queue_t *run_process(p_queue_t *old_queue, sched_info_t sched_info);
p_queue_t *kill_process(int32_t pid, p_queue_t *queue);

#endif
