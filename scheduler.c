#include "scheduler.h"
#include "mmu.h"

process_t *create_process(int mem_size, uint8_t pid) {
    process_t *new_process = malloc(sizeof(process_t));
    new_process->pid = pid;
    new_process->mem_size = mem_size;
    new_process->mem_start = first_fit_mem(mem_size);
    new_process->state = READY;
    new_process->time_quantum = 0;
    new_process->time_remaining = 10;
    new_process->time_used = 0;
    new_process->time_waiting = 0;
    return new_process;
}

p_queue_t *add_process_to_queue(p_queue_t *old_queue,
                                process_t *process_to_add) {
    p_queue_t *new_queue = malloc(sizeof(p_queue_t));
    p_queue_t *current = old_queue;

    new_queue->process = process_to_add;
    new_queue->next = NULL;

    if (old_queue == NULL) {
        new_queue->prev = NULL;
        return new_queue;
    }

    while (current->next != NULL) {
        current = current->next;
    }

    new_queue->prev = current;
    current->next = new_queue;
    return old_queue;
}

p_queue_t *run_process(p_queue_t *old_queue, sched_info_t sched_info) {
    p_queue_t *current = old_queue;

    if (old_queue == NULL) {
        return NULL;
    }

    while (current->process->state != READY &&
           current->process->state != RUNNING) {
        if (current->next == NULL) {
            return old_queue;
        }
        current = current->next;
    }

    if (sched_info.algorithm == FIFO) {
        if (current->process->time_remaining > 0) {
            current->process->time_remaining--;
            current->process->time_used++;
            current->process->state = RUNNING;
            return old_queue;
        } else {
            /* current->process->state = TERMINATED; */
            return kill_process(current->process->pid, old_queue);
        }
    } else {
        if (current->process->time_remaining <= 0) {
            /* current->process->state = TERMINATED; */
            return kill_process(current->process->pid, old_queue);
        } 

        if (current->process->time_quantum < sched_info.time_quantum) {
            current->process->time_quantum++;
            current->process->time_remaining--;
            current->process->time_used++;
            current->process->state = RUNNING;
            return old_queue;
        } else {
            current->process->time_quantum = 0;
            current->process->state = READY;
            return old_queue;
        }
    }
}

p_queue_t *kill_process(int32_t pid, p_queue_t *queue) {
    p_queue_t *current = queue;

    if (queue == NULL) {
        return NULL;
    }

    while (current->process->pid != pid) {
        current = current->next;
        if (current == NULL) {
            return queue;
        }
    }

    /*
    current->prev->next = current->next;
    current->next->prev = current->prev;
    free(current);
    */
    current->process->state = TERMINATED;
    free_mem(current->process->mem_start, current->process->mem_size);
    return queue;
}
