#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "airport.h"

static char *format(char *buffer, flight_t *f) {
  int delay = f->completed - f->departure - f->length - 20;
  sprintf(buffer, 
          "[%2.2d:%2.2d] %s %d from %s to %s, departed %2.2d:%2.2d, delay %d.",
          f->completed / 60, f->completed % 60, f->airline, f->f_no, 
          airport_name(f->origin), airport_name(f->destination), 
          f->departure / 60, f->departure % 60, delay);

  return buffer;
}

static int compare(const void *f, const void *g) {
  flight_t *ft1 = *(flight_t **)f;
  flight_t *ft2 = *(flight_t **)g;
  int diff;

  assert(ft1);
  assert(ft2);

  diff = ft1->completed - ft2->completed;
  if (!diff) {
    diff = strcmp(ft1->airline, ft2->airline);
    if (!diff) {
      diff = ft1->f_no - ft2->f_no;
    }
  } 
  return diff;
}

extern void atcprint(flight_t **flights, int size) {
  int i;
  char buffer[100];

  qsort(flights, size, sizeof(flight_t *), &compare);

  for (i = 0; i < size; i++) {
    fprintf(stdout, "%s\n", format(buffer,flights[i]));
  }
}

