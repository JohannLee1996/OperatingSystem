#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

typedef struct node_ node;
struct node_ {
  node *next;
  void *data;
  int  prio;
};

typedef struct qrec_ qrec;
struct qrec_ {
  node *head;
  node *tail;
  pthread_mutex_t lock;
};

typedef void *pqueue_t;

extern pqueue_t pqueue_new();
extern void pqueue_enqueue(pqueue_t q, int prio, void *data);
extern void *pqueue_dequeue(pqueue_t q);
extern void *pqueue_peek(pqueue_t q, int *prio);
extern int pqueue_size(pqueue_t q);

#endif
