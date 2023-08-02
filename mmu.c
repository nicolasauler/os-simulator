#include "mmu.h"

static bool mem[MAXMEM] = {false};

void init_mem(void) {
    int i;
    for (i = 0; i < MAXMEM; i++) {
        mem[i] = false;
    }
}

int8_t first_fit_mem(int mem_size) {
    int8_t i, j;
    bool found = false;

    /* checks if there is a contiguous block of memory of size mem_size
     * the  block can overlap from MAXMEM back to the beggining */
    for (i = 0; i < MAXMEM; i++) {
        if (mem[i] == false) {
            found = true;
            for (j = i; j < i + mem_size; j++) {
                if (mem[j % MAXMEM] == true) {
                    found = false;
                    break;
                }
            }
        }
        if (found) {
            break;
        }
    }

    if (found) {
        for (j = i; j < i + mem_size; j++) {
            mem[j % MAXMEM] = true;
        }
        return i;
    } else {
        return -1;
    }
}

bool *get_memory_map() {
    bool *mem_clone = malloc(sizeof(bool) * MAXMEM);
    int i;
    for (i = 0; i < MAXMEM; i++) {
        mem_clone[i] = mem[i];
    }
    return mem_clone;
}

void free_mem(int8_t mem_start, int mem_size) {
    int i;
    for (i = mem_start; i < mem_start + mem_size; i++) {
        mem[i] = false;
    }
}
