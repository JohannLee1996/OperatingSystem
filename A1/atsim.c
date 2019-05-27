#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include "airport.c"

int main(int argc, char *argv[])
{
  FILE *fp;
  char strLine[30];
  queue *qptr;
  airqueue *aqptr;

  if ((fp = fopen(argv[1], "r")) == NULL)
  {
    perror("fopen");
    exit(0);
  }

  qptr = newqueue();
  aqptr = newairqueue();

  fgets(strLine, 30, fp);
  while (strcmp(strLine, "end\n"))
  {
    flight *tempf;
    tempf = loadFlight(strLine);
    add_airports(aqptr, tempf);
    enqueue(qptr, tempf);
    fgets(strLine, 30, fp);
  }

  add_flight_to_depart(aqptr, qptr);

  int t = 0;
  while (1)
  {
    sim_take_off(t, aqptr);
    sim_landing(t, aqptr);
    sim_enroute(t, aqptr);
    sim_output(t, aqptr);
    if (t == 1440)
    {
      break;
    }
    t++;
  }

  airport *add = aqptr->front;
  int total = 0;
  while (add!=NULL)
  {
    //printf("%d, %d, %d, %d\n",add->takingoffQueue->count,add->landingQueue->count,add->enrouteQueue->count,add->waitingQueue->count);
    total += add->takingoffQueue->count;
    total += add->landingQueue->count;
    total += add->enrouteQueue->count;
    total += add->waitingQueue->count;
    add=add->next;
  }
  

  fclose(fp);

  return 0;
}
