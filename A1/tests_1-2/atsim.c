#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include "airport.c"

int main(int argc, char *argv[])
{
  //FILE *fp;
  char strLine[30];
  queue *qptr;
  airqueue *aqptr;

  // if ((fp = fopen(argv[1], "r")) == NULL)
  // {
  //   perror("fopen");
  //   exit(0);
  // }

  qptr = newqueue();
  aqptr = newairqueue();

  fgets(strLine, 30, stdin);
  while (strcmp(strLine, "end\n"))
  {
    flight *tempf;
    tempf = loadFlight(strLine);
    add_airports(aqptr, tempf);
    enqueue(qptr, tempf, 0);
    fgets(strLine, 30, stdin);
  }

  add_flight_to_depart(aqptr, qptr);

  int t = 0;
  while (1)
  {
    sim_take_off(t, aqptr);
    sim_landing(t, aqptr);
    take_turns(t, aqptr, qptr);
    if (t == 1440)
    {
      break;
    }
    t++;
  }
  flight *tempf = qptr->front;
  while (tempf != NULL)
  {
    int h = (tempf->landing_time) / 60;
    int m = (tempf->landing_time) - 60 * h;
    printf("[%02d:%02d] %s %d from %s to %s, departed %02d:%02d, delay %d.\n", h, m, tempf->C, tempf->N, tempf->O, tempf->D, tempf->H, tempf->M, tempf->delay);
    tempf = tempf->next;
  }

  //fclose(fp);

  return 0;
}
