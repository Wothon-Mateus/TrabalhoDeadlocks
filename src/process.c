#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "bank.h"
#include "process.h"
#include "sleep-util.h"

typedef struct process {
  int r;
  int pid;
  bool can_run;
  int *max;
  int *request;
  struct bank *bank;
} process;

process *process_create(int pid, int r, int *max, struct bank *b) {
  process *p = (process *)malloc(sizeof(process));
  p->pid = pid;
  p->r = r;
  p->bank = b;
  p->can_run = false;
  p->max = (int *)malloc(r * sizeof(int));
  p->request = (int *)malloc(r * sizeof(int));
  memcpy(p->max, max, r * sizeof(int));
  return p;
}

void *process_run(void *arg) {
  process *p = (process *)arg;
  p->can_run = true;
  srand(time(NULL) + p->pid); // Seed random number generator

  while (p->can_run) {
    nap();

    for (int i = 0; i < p->r; i++) {
      p->request[i] = rand() % (p->max[i] + 1);
    }

    if (bank_request_resources(p->bank, p->pid, p->request)) {
      nap();
      bank_release_resources(p->bank, p->pid, p->request);
    }
  }
  printf("Thread # %d I'm interrupted.\n", p->pid);
  free(p->max);
  free(p->request);
  free(p);
  return NULL;
}

void process_stop(process *p) { p->can_run = false; }
