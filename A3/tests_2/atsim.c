#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/errno.h>
#include "atcprint.h"
#include "airport.h"
// #include "atcprint.c"
// #include "airport2.c"

#define ONE_DAY (24 * 60)

int num = 0;
int n = 0;
flight_t **completed;
int waiting = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void error(char *err)
{
  fprintf(stderr, "%s : %s\n", err, strerror(errno));
  abort();
}

static void run(airport_t apt);

static void barrier()
{
  pthread_mutex_lock(&mtx);
  waiting++;
  if (waiting < airport_num())
  {
    pthread_cond_wait(&cond, &mtx);
  }
  else
  {
    waiting = 0;
    pthread_cond_broadcast(&cond);
  }
  pthread_mutex_unlock(&mtx);
}

int main(int argc, char **argv)
{
  airport_t apt;
  int hour = 0;
  int minute = 0;
  int num_ap = 0;
  char airline[20];
  char origin[20];
  char dest[20];
  flight_t *flight;

  // FILE *fp;
  // if ((fp = fopen(argv[1], "r")) == NULL)
  // {
  //   perror("fopen");
  //   exit(0);
  // }

  while (1)
  {
      if (scanf("%s", airline) != 1) {
        error("Unexpected scanf failure");
      } else if (!strcmp(airline, "end")) {
        break;
      }

    // fscanf(fp, "%s", airline);
    // if (!strcmp(airline, "end"))
    // {
    //   break;
    // }

    flight = malloc(sizeof(flight_t));
    assert(flight);
    memset(flight, 0, sizeof(flight_t));

    if ((scanf("%d", &flight->f_no) != 1) || (scanf("%d", &flight->pid) != 1) ||
        (scanf("%s", origin) != 1) || (scanf("%d:%d", &hour, &minute) != 2) ||
        (scanf("%d", &flight->length) != 1) || (scanf("%s", dest) != 1)) {
      error("Unexpected scanf failure");
    }

    // if ((fscanf(fp, "%d", &flight->f_no) != 1) || (fscanf(fp, "%d", &flight->pid) != 1) ||
    //     (fscanf(fp, "%s", origin) != 1) || (fscanf(fp, "%d:%d", &hour, &minute) != 2) ||
    //     (fscanf(fp, "%d", &flight->length) != 1) || (fscanf(fp, "%s", dest) != 1))
    // {
    //   error("Unexpected scanf failure");
    // }

    strcpy(flight->airline, airline);
    flight->origin = airport_get(origin);
    flight->departure = 60 * hour + minute;
    flight->destination = airport_get(dest);
    airport_schedule(flight);
    num++;
  }

  completed = malloc(sizeof(flight_t *) * num);
  assert(completed);

  num_ap = airport_num();
  pthread_t my_thread[num_ap];
  int iter = 0;
  for (apt = airport_next(NULL); apt != NULL; apt = airport_next(apt))
  {
    int ret;
    ret = pthread_create(&my_thread[iter], NULL, (void *)&run, apt);
    if (ret != 0)
    {
      printf("Create thread fail!\n");
    }
    // else
    // {
    //   printf("Thread created!\n");
    // }
    iter++;
  }

  // for (int i = 0; i < num_ap; i++)
  // {
  //   pthread_join(my_thread[i], NULL);
  // }
  usleep(20000);
  

  atcprint(completed, n);
  return 0;
}

static void run(airport_t apt)
{
  flight_t *flight;
  for (int i = 0; (n < num) && (i < ONE_DAY); i++) // barrier on i
  {
    flight = airport_step(apt, i); // wait for all thread to complete at time i
    if (flight)
    {
      completed[n] = flight;
      n++;
    }
    barrier();
  }
}