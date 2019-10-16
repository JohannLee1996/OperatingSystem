#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "pqueue.h"
#include "utils.h" 

static pthread_mutex_t free_lock = PTHREAD_MUTEX_INITIALIZER;
static node *freelist;

static node *alloc_node(int prio, void *data) {

  lock(&free_lock);
  node *nd = freelist;
  if (nd) {
    freelist = nd->next;
  } else {
    nd = malloc(sizeof(node));
  }
  unlock(&free_lock);

  assert(nd);
  memset(nd, 0, sizeof(node));
  nd->data = data;
  nd->prio = prio;
  return nd;
}

static void free_node(node *nd) {
  assert(nd);

  lock(&free_lock);
  nd->next = freelist;
  freelist = nd;
  unlock(&free_lock);
}

extern pqueue_t pqueue_new() {
  qrec *q = malloc(sizeof(qrec));
  assert(q);
  memset(q, 0, sizeof(qrec));
  if (pthread_mutex_init(&q->lock, NULL)) {
    error("Could not initialize mutex lock" );
  }
  return q;
}

extern void pqueue_enqueue(pqueue_t q, int prio, void *data) {
  qrec *qr = (qrec *)q;
  node *nd = alloc_node(prio, data);
  node *tmp;

  lock(&qr->lock);
  if (!qr->head) {
    qr->head = nd;
    qr->tail = nd;
  } else if (qr->head->prio > prio) {
    nd->next = qr->head;
    qr->head = nd;
  } else {
    for (tmp = qr->head; tmp->next; tmp = tmp->next) {
      if (prio < tmp->next->prio) {
        break;
      }
    }
    
    nd->next = tmp->next;
    tmp->next = nd;
    if (tmp == qr->tail) {
      qr->tail = nd;
    }
  }
  unlock(&qr->lock);
}

extern void *pqueue_dequeue(pqueue_t q) {
  qrec *qr = (qrec *)q;
  void *data = NULL;
  node *tmp;

  lock(&qr->lock);
  if (qr->head) {
    tmp = qr->head;
    qr->head = qr->head->next;
    data = tmp->data;
    free_node(tmp);
  }
  unlock(&qr->lock);
  return data;
}

extern void *pqueue_peek(pqueue_t q, int *prio) {
  qrec *qr = (qrec *)q;
  void *data = NULL;

  assert(prio);

  lock(&qr->lock);
  if (qr->head) {
    data = qr->head->data;
    if (prio) {
      *prio = qr->head->prio;
    }
  }
  unlock(&qr->lock);
  return data;
}

extern int pqueue_size(pqueue_t q){// a new method used to get the size of queue
  int i = 0;
  node *nd;
  qrec *qr = (qrec *)q;
  for(nd = qr->head;nd; nd = nd->next){
    i++;
  }
  return i;
}
