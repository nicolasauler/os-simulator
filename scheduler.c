#include "scheduler.h"
#include "mmu.h"

p_queue_t *move_to_end_of_queue(p_queue_t *old_queue) {
    p_queue_t *current = old_queue;

    if (old_queue == NULL) {
        return NULL;
    }

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = old_queue;
    old_queue->prev = current;
    old_queue = old_queue->next;
    old_queue->prev = NULL;
    current->next->next = NULL;
    return old_queue;
}

process_t *create_process(int mem_size, uint8_t pid) {
    process_t *new_process = malloc(sizeof(process_t));
    new_process->pid = pid;
    new_process->mem_size = mem_size;
    new_process->mem_start = -1;
    new_process->state = NEW;
    new_process->time_quantum = 0;
    new_process->time_remaining = 10;
    new_process->time_used = 0;
    new_process->time_waiting = 0;
    return new_process;
}

bool can_allocate_mem(process_t **process) {
    int8_t mem_start = first_fit_mem((*process)->mem_size);

    if (mem_start == -1) {
        return false;
    }

    (*process)->mem_start = mem_start;
    return true;
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

p_queue_t *toggle_next_process_state(p_queue_t *queue) {
    p_queue_t *current = queue;

    if (queue == NULL) {
        return NULL;
    }

    current->process->state = RUNNING;
    return queue;
}

p_queue_t *run_process(p_queue_t *old_queue, sched_info_t sched_info) {
    p_queue_t *current = old_queue;

    if (old_queue == NULL) {
        return NULL;
    }

    if (current->process->mem_start == -1) {
        if (can_allocate_mem(&(current->process))) {
            current->process->state = READY;
        } else {
            current->process->time_waiting++;
            return move_to_end_of_queue(old_queue);
        }
    }

    if (sched_info.algorithm == FIFO) {
        if (current->process->time_remaining > 0) {
            current->process->time_remaining--;
            current->process->time_used++;
            current->process->state = RUNNING;
        }

        if (current->process->time_remaining > 0) {
            return old_queue;
        }

        return kill_process(current->process->pid, old_queue);
    } else {
        if (current->process->time_quantum < sched_info.time_quantum) {
            current->process->time_quantum++;
            current->process->time_remaining--;
            current->process->time_used++;
            current->process->state = RUNNING;
        }

        if (current->process->time_remaining > 0) {
            if (current->process->time_quantum >= sched_info.time_quantum) {
                current->process->time_quantum = 0;
                current->process->state = READY;
                old_queue = move_to_end_of_queue(old_queue);
                old_queue = toggle_next_process_state(old_queue);
            }

            return old_queue;
        }

        return kill_process(current->process->pid, old_queue);
    }
}

p_queue_t *remove_process_from_queue(p_queue_t *queue, p_queue_t *process) {
    p_queue_t *current = queue;

    if (process->prev == NULL) {
        queue = process->next;
        if (queue != NULL) {
            queue->prev = NULL;
        }
        free(process);
        return queue;
    }

    while (current->next != process) {
        current = current->next;
    }

    current->next = process->next;
    if (process->next != NULL) {
        process->next->prev = current;
    }
    free(process);
    return queue;
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

    free_mem(current->process->mem_start, current->process->mem_size);
    queue = remove_process_from_queue(queue, current);
    /*
    current->process->state = TERMINATED;
    */
    return queue;
}
