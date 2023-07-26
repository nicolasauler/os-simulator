#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

process_t *create_process(int mem_size, uint8_t *pid) {
    process_t *new_process = malloc(sizeof(process_t));
    new_process->pid = (*pid)++;
    new_process->mem_size = mem_size;
    new_process->mem_start = 0;
    new_process->state = NEW;
    new_process->time_quantum = 0;
    new_process->time_remaining = 10;
    new_process->time_used = 0;
    new_process->time_waiting = 0;
    return new_process;
}

void print_process(process_t *process) {
    printf("pid: %d\n", process->pid);
    printf("mem_size: %d\n", process->mem_size);
    printf("mem_start: %d\n", process->mem_start);
    printf("state: %d\n", process->state);
    printf("time_quantum: %d\n", process->time_quantum);
    printf("time_remaining: %d\n", process->time_remaining);
    printf("time_used: %d\n", process->time_used);
    printf("time_waiting: %d\n", process->time_waiting);
    printf("\n");
}

p_circ_queue_t *add_process_to_queue(p_circ_queue_t *old_queue,
                                     process_t *process_to_add) {
    p_circ_queue_t *new_queue = malloc(sizeof(p_circ_queue_t));
    new_queue->process = process_to_add;

    if (old_queue == NULL) {
        new_queue->next = new_queue;
        new_queue->prev = new_queue;
        return new_queue;
    }

    p_circ_queue_t *current = old_queue;
    while (current->next != old_queue) {
        current = current->next;
    }
    current->next = new_queue;
    new_queue->prev = current;
    new_queue->next = old_queue;
    old_queue->prev = new_queue;
    return old_queue;
}

p_circ_queue_t *run_process(p_circ_queue_t *queue) {
    if (queue == NULL) {
        return NULL;
    }

    p_circ_queue_t *current = queue;
    if (current->process->time_remaining > 0) {
        current->process->time_remaining--;
        current->process->time_used++;
        return current;
    } else {
        while (current->next != queue) {
            current = current->next;
        }
        current->next = queue->next;
        queue->next->prev = current;
        free(queue);
        return current->next;
    }
}

int main(void) {
    uint8_t pid = 0;
    p_circ_queue_t *queue = NULL;

    process_t *p1 = create_process(10, &pid);
    queue = add_process_to_queue(queue, p1);
    print_process(queue->process);

    process_t *p2 = create_process(20, &pid);
    queue = add_process_to_queue(queue, p2);
    print_process(queue->next->process);

    queue = run_process(queue);
    print_process(queue->process);

    return (0);
}
