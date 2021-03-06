#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include "atcprint.h"
#include "airport.h"
#include "atcprint.c"
#include "airport2.c"
#include "utils.h"
#include "utils.c"

#define ONE_DAY (24 * 60)

static flight_t **completed;
static volatile int done = 0;
static int num = 0;

static void * run_airport(void *data) {
  airport_t apt = data;
  flight_t *flight;
  int i;
  static pthread_mutex_t a_lock = PTHREAD_MUTEX_INITIALIZER;

  set_inbox(apt, comm_allocate());
  set_response(apt, comm_allocate());
  barrier();

  for (i = 0; i < ONE_DAY; i++) {
    flight = airport_step(apt, i);
    if (flight) {
      lock(&a_lock);
      completed[done] = flight;
      done++;
      unlock(&a_lock);
    }
    barrier();
  }
  return NULL;
}

int main(int argc, char **argv) {
  airport_t apt;
  int hour = 0;
  int minute = 0;
  int i = 0;
  char airline[20];
  char origin[20];
  char dest[20];
  flight_t *flight;
  pthread_t *tid;

  FILE *fp;
  if ((fp = fopen(argv[1], "r")) == NULL)
  {
    perror("fopen");
    exit(0);
  }

  while(1) {
    // if (scanf("%s", airline) != 1) {
    //   error("Unexpected scanf failure");
    // } else if (!strcmp(airline, "end")) {
    //   break;
    // } 

    fscanf(fp, "%s", airline);
    if (!strcmp(airline, "end"))
    {
      break;
    }
 
    flight = malloc(sizeof(flight_t));
    assert(flight);
    memset(flight, 0, sizeof(flight_t));

    // if ((scanf("%d", &flight->f_no) != 1) || (scanf("%d", &flight->pid) != 1) ||
    //     (scanf("%s", origin) != 1) || (scanf("%d:%d", &hour, &minute) != 2) || 
    //     (scanf("%d", &flight->length) != 1) || (scanf("%s", dest) != 1)) {
    //   error("Unexpected scanf failure");
    // }

    if ((fscanf(fp, "%d", &flight->f_no) != 1) || (fscanf(fp, "%d", &flight->pid) != 1) ||
        (fscanf(fp, "%s", origin) != 1) || (fscanf(fp, "%d:%d", &hour, &minute) != 2) ||
        (fscanf(fp, "%d", &flight->length) != 1) || (fscanf(fp, "%s", dest) != 1))
    {
      error("Unexpected scanf failure");
    }
  
    strcpy(flight->airline, airline);
    flight->origin = airport_get(origin);
    flight->departure = 60 * hour + minute;
    flight->destination = airport_get(dest);
    airport_schedule(flight);
    num++;
  }

  completed = malloc(sizeof(flight_t *) * num);
  assert(completed);

  threads = airport_num();
  tid = malloc(sizeof(pthread_t) * threads);
  assert(tid);

  apt = NULL;
  for (i = 0; i < threads; i++) {
    apt = airport_next(apt);
    if (pthread_create( &tid[i], NULL, run_airport, apt)) {
      error("Could not create thread\n");
    }
  }

  for (i = 0; i < threads; i++) {
    if (pthread_join(tid[i], NULL)) {
      error("error: could not join thread\n");
    }
  }
  // usleep(20000);

  atcprint(completed, done);
  return 0;
}
