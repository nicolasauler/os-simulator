#ifndef _MMU_H_
#define _MMU_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MAXMEM 20

void init_mem(void);
int8_t first_fit_mem(int mem_size);
bool *get_memory_map(void);
void free_mem(int8_t mem_start, int mem_size);

#endif
