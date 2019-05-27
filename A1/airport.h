#ifndef airport_h
#define airport_h

#include "pqueue.h"

struct airport{
    char code[4];
    int flag;
    queue * takingoffQueue;
    queue * landingQueue;
    queue * enrouteQueue;
    queue * waitingQueue;

    struct airport * next;
};
typedef struct airport airport;

struct airqueue{
  int count;
  airport *front;
  airport *rear;
};
typedef struct airqueue airqueue;

#endif