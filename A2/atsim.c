#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include "atcprint.h"
#include "airport.h"
// #include "atcprint.c"
// #include "airport.c"
// #include "pqueue.c"

#define ONE_DAY (24 * 60)

static void error(char *err) {
  fprintf(stderr, "%s : %s\n", err, strerror(errno));
  abort();
}

int main(int argc, char **argv) {
  airport_t apt;
  int hour = 0;
  int minute = 0;
  int num = 0;
  int n = 0;
  char airline[20];
  char origin[20];
  char dest[20];
  flight_t **completed;
  flight_t *flight;

  // FILE *fp;
  // if ((fp = fopen(argv[1], "r")) == NULL)
  // {
  //   perror("fopen");
  //   exit(0);
  // }

  while(1) {
    if (scanf("%s", airline) != 1) {
      error("Unexpected scanf failure");
    } else if (!strcmp(airline, "end")) {
      break;
    } 
    
    // fscanf(fp, "%s", airline);
    // if(!strcmp(airline, "end")){
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
    //     (fscanf(fp, "%d", &flight->length) != 1) || (fscanf(fp, "%s", dest) != 1)) {
    //   error("Unexpected scanf failure");
    // }
  
    status_check(flight->pid);
    strcpy(flight->airline, airline);
    flight->origin = airport_get(origin);
    flight->departure = 60 * hour + minute;
    flight->destination = airport_get(dest);
    airport_schedule(flight);
    num++;
  }

  completed = malloc(sizeof(flight_t *) * num);
  assert(completed);

  for (int i = 0; (n < num) && (i < ONE_DAY); i++) {
    for (apt = airport_next(NULL); apt != NULL; apt = airport_next(apt)) {
      flight = airport_step(apt, i);
      if (flight) {
        completed[n] = flight;
        n++;
      }
    }
  }

  atcprint(completed, n);
  return 0;
}
