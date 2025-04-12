#include "sleep-util.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NAP_TIME 5000000

static void nap_duration(int duration) {
  srand(time(NULL)); // Seed the random number generator
  int sleeptime =
      rand() %
      (duration + 1); // Generate random sleep time between 0 and duration
  usleep(sleeptime);  // usleep function takes microseconds as argument
}

void nap() { nap_duration(NAP_TIME); }
