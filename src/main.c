#include "bank.h"
#include "process.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

volatile sig_atomic_t ctrl_c_pressed = 0; // Flag to indicate CTRL+C

void ctrl_c_handler(int signum) {
  if (signum == SIGINT) {
    printf("\nCTRL+C pressed!\n");
    ctrl_c_pressed = 1; // Set the flag
  }
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <one or more resources> <input file>\n",
            argv[0]);
    return -1;
  }

  struct sigaction sa;
  sa.sa_handler = ctrl_c_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction");
    return 1;
  }

  int r = argc - 2; // total of resources
  char *file = argv[argc - 1];
  int resources[r];

  for (int i = 0; i < r; i++) {
    resources[i] = atoi(argv[i + 1]);
  }

  struct bank *b = bank_create(resources, r, PROCESS_COUNT);
  int max[r];
  pthread_t workers[PROCESS_COUNT];
  struct process *process[PROCESS_COUNT];

  FILE *inFile = fopen(file, "r");
  if (inFile == NULL) {
    fprintf(stderr, "Unable to find file \"%s\"\n", file);
    return -1;
  }

  char line[256];
  int pid = 0;
  for (int i = 0; i < PROCESS_COUNT; i++) {
    if (fgets(line, sizeof(line), inFile) == NULL) {
      fprintf(stderr, "Error reading from \"%s\"\n", file);
      fclose(inFile);
      return -1;
    }

    char *token = strtok(line, ",");
    int rid = 0;
    while (token != NULL) {
      max[rid++] = atoi(token);
      token = strtok(NULL, ",");
    }

    // CRIAR PROCESSO, INICIAR THREAD DO PROCESSO E ADICIONAR PROCESSO AO BANCO
    process[i] = process_create(pid, r, max, b);
    bank_add_process(b, pid, max);
    pthread_create(&workers[i], NULL, process_run, (void *)process[i]);
    pid++;

  }

  fclose(inFile);
  printf("MAIN: started threads\n");

  while (!ctrl_c_pressed) {
    bank_print_state(b);
    sleep(2);
  }

  printf("MAIN: trying to stop threads, can take many  seconds...\n\n");
  for (int i = 0; i < PROCESS_COUNT; i++) {
    process_stop(process[i]);
    pthread_join(workers[i], NULL);
  }
  printf("MAIN: final bank state:\n");
  bank_print_state(b);
  bank_destroy(b);
  printf("MAIN: all done, exiting.\n");
  return 0;
}
