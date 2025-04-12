#ifndef PROCESS_H
#define PROCESS_H

#include "bank.h"

#ifndef PROCESS_COUNT
#define PROCESS_COUNT 5
#endif

struct process;

// creates a new process with the provided `pid` for `r` resources
// with up to `max` instances of each resource and considering the
// provided `bank`
struct process *process_create(int pid, int r, int *max, struct bank *b);
// represents the process thread function, with `p` of type `struct process`
void *process_run(void *p);
// stops process `p` execution and frees allocated memory
void process_stop(struct process *p);

#endif
