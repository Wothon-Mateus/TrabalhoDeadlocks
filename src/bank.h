#ifndef BANK_H
#define BANK_H

#include <stdbool.h>

struct bank;

// creates a new bank with `r` resources and `p` processes.
// `resources` represent the max number of instances for each resource
struct bank *bank_create(int *resources, int r, int p);
// destroys (frees) the memory associated with the bank
void bank_destroy(struct bank *b);
// adds the process identified by `pid`, with maximum demand `max`
// for `r` resources
void bank_add_process(struct bank *b, int pid, int *max);
// prints the bank state
void bank_print_state(struct bank *b);
// tries to provide process `pid` the `request` for `r` resources
bool bank_request_resources(struct bank *b, int pid, int *request);
// releases the `release` resources in use by process `pid`
void bank_release_resources(struct bank *b, int pid, int *release);

#endif // BANK_H
