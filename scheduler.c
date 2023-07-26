#include "scheduler.h"
#include <stdlib.h>

process_t *create_process(int mem_size, uint8_t pid) {
    process_t *new_process = malloc(sizeof(process_t));
    new_process->pid = pid;
    new_process->mem_size = mem_size;
    new_process->mem_start = 0;
    new_process->state = NEW;
    new_process->time_quantum = 0;
    new_process->time_remaining = 10;
    new_process->time_used = 0;
    new_process->time_waiting = 0;
    return new_process;
}

p_circ_queue_t *add_process_to_queue(p_circ_queue_t *old_queue,
                                     process_t *process_to_add) {
    p_circ_queue_t *new_queue = malloc(sizeof(p_circ_queue_t));
    p_circ_queue_t *current = old_queue;

    new_queue->process = process_to_add;

    if (old_queue == NULL) {
        new_queue->next = new_queue;
        new_queue->prev = new_queue;
        return new_queue;
    }

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
    p_circ_queue_t *current = queue;

    if (queue == NULL) {
        return NULL;
    }

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

void kill_process(uint8_t pid, p_circ_queue_t *queue) {
    p_circ_queue_t *current = queue;

    if (queue == NULL) {
        return;
    }

    while (current->process->pid != pid) {
        current = current->next;
    }
    current->prev->next = current->next;
    current->next->prev = current->prev;
    free(current);
}
