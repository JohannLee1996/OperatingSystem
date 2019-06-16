#ifndef QUEUE_H
#define QUEUE_H

typedef void *pqueue_t;

extern pqueue_t pqueue_new();
extern void pqueue_enqueue(pqueue_t q, int prio, void *data);
extern void *pqueue_dequeue(pqueue_t q);
extern void *pqueue_peek(pqueue_t q, int *prio);

#endif
