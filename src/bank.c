// Wothon Mateus de Araujo
// Matricula: 12111BSI262

#include "bank.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct bank
{
  int p;
  int r;
  int *available;
  int **maximum;
  int **allocation;
  int **need;
  pthread_mutex_t mut;
} bank;

bank *bank_create(int *resources, int r, int p)
{
  bank *b = (bank *)malloc(sizeof(bank));
  if (b == NULL)
    return NULL;

  b->r = r;
  b->p = p;
  pthread_mutex_init(&b->mut, NULL);

  b->available = (int *)malloc(sizeof(int) * r);
  if (b->available == NULL)
  {
    free(b);
    return NULL;
  }
  memcpy(b->available, resources, sizeof(int) * r);

  b->maximum = (int **)malloc(sizeof(int *) * p);
  b->allocation = (int **)malloc(sizeof(int *) * p);
  b->need = (int **)malloc(sizeof(int *) * p);

  if (b->maximum == NULL || b->allocation == NULL || b->need == NULL)
  {
    free(b->available);
    free(b->maximum);
    free(b->allocation);
    free(b->need);
    free(b);
    return NULL;
  }

  for (int i = 0; i < p; i++)
  {
    b->maximum[i] = NULL;
    b->allocation[i] = NULL;
    b->need[i] = NULL;
  }

  return b;
}

void bank_destroy(bank *b)
{
  if (b != NULL)
  {
    if (b->available)
      free(b->available);

    for (int i = 0; i < b->p; i++)
    {
      if (b->maximum[i])
        free(b->maximum[i]);
      if (b->allocation[i])
        free(b->allocation[i]);
      if (b->need[i])
        free(b->need[i]);
    }

    free(b->maximum);
    free(b->allocation);
    free(b->need);

    pthread_mutex_destroy(&b->mut);
    free(b);
  }
}

void bank_add_process(bank *bank, int pid, int *max)
{
  bank->maximum[pid] = (int *)malloc(sizeof(int) * bank->r);
  bank->allocation[pid] = (int *)malloc(sizeof(int) * bank->r);
  bank->need[pid] = (int *)malloc(sizeof(int) * bank->r);

  memcpy(bank->maximum[pid], max, sizeof(int) * bank->r);
  memcpy(bank->need[pid], max, sizeof(int) * bank->r);
  memset(bank->allocation[pid], 0, sizeof(int) * bank->r);
}

void bank_print_state(bank *bank)
{
  pthread_mutex_lock(&bank->mut);
  printf("\n\n---------BANK STATE------------\n");
  printf("Available  = [");
  for (int i = 0; i < bank->r; i++)
  {
    printf("%d%s", bank->available[i], (i == bank->r - 1) ? "]" : " ");
  }

  printf("\nAllocation = ");
  for (int i = 0; i < bank->p; i++)
  {
    printf("[");
    for (int j = 0; j < bank->r; j++)
    {
      printf("%d%s", bank->allocation[i][j], (j == bank->r - 1) ? "]" : " ");
    }
  }

  printf("\nMax        = ");
  for (int i = 0; i < bank->p; i++)
  {
    printf("[");
    for (int j = 0; j < bank->r; j++)
    {
      printf("%d%s", bank->maximum[i][j], (j == bank->r - 1) ? "]" : " ");
    }
  }

  printf("\nNeed       = ");
  for (int i = 0; i < bank->p; i++)
  {
    printf("[");
    for (int j = 0; j < bank->r; j++)
    {
      printf("%d%s", bank->need[i][j], (j == bank->r - 1) ? "]" : " ");
    }
  }

  printf("\n-------------------------------\n\n");
  pthread_mutex_unlock(&bank->mut);
}

static bool bank_is_safe_state(bank *b, int pid, int *request)
{
  printf("\nPROCESS #%d requesting ", pid);
  for (int i = 0; i < b->r; i++)
    printf("%d ", request[i]);

  // Verificar necessidade
  for (int i = 0; i < b->r; i++)
  {
    if (request[i] > b->need[pid][i] || request[i] > b->available[i])
      return false;
  }

  // Simular alocação
  int *work = malloc(b->r * sizeof(int));
  bool *finish = calloc(b->p, sizeof(bool));

  for (int i = 0; i < b->r; i++)
  {
    work[i] = b->available[i] - request[i];
    b->allocation[pid][i] += request[i];
    b->need[pid][i] -= request[i];
  }

  bool safe = true;

  while (true)
  {
    bool found = false;
    for (int i = 0; i < b->p; i++)
    {
      if (!finish[i])
      {
        bool can_finish = true;
        for (int j = 0; j < b->r; j++)
        {
          if (b->need[i][j] > work[j])
          {
            can_finish = false;
            break;
          }
        }

        if (can_finish)
        {
          for (int j = 0; j < b->r; j++)
            work[j] += b->allocation[i][j];
          finish[i] = true;
          found = true;
        }
      }
    }
    if (!found)
      break;
  }

  for (int i = 0; i < b->p; i++)
  {
    if (!finish[i])
    {
      safe = false;
      break;
    }
  }

  // Rollback da simulação
  for (int i = 0; i < b->r; i++)
  {
    b->allocation[pid][i] -= request[i];
    b->need[pid][i] += request[i];
  }

  free(work);
  free(finish);
  return safe;
}

bool bank_request_resources(bank *bank, int pid, int *request)
{
  pthread_mutex_lock(&bank->mut);

  if (!bank_is_safe_state(bank, pid, request))
  {
    pthread_mutex_unlock(&bank->mut);
    return false;
  }

  for (int i = 0; i < bank->r; i++)
  {
    bank->available[i] -= request[i];
    bank->allocation[pid][i] += request[i];
    bank->need[pid][i] = bank->maximum[pid][i] - bank->allocation[pid][i];
  }

  pthread_mutex_unlock(&bank->mut);
  return true;
}

void bank_release_resources(bank *bank, int pid, int *release)
{
  pthread_mutex_lock(&bank->mut);

  for (int i = 0; i < bank->r; i++)
  {
    bank->available[i] += release[i];
    bank->allocation[pid][i] -= release[i];
    bank->need[pid][i] += release[i];
  }

  pthread_mutex_unlock(&bank->mut);
}
