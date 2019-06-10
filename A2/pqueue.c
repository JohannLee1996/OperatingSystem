#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "pqueue.h"


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
};
 

static node *freelist;

static node *alloc_node(int prio, void *data) {
  node *nd = freelist;
  if (nd) {
    freelist = nd->next;
  } else {
    nd = malloc(sizeof(node));
  }
  assert(nd);
  memset(nd, 0, sizeof(node));
  nd->data = data;
  nd->prio = prio;
  return nd;
}

static void free_node(node *nd) {
  assert(nd);

  nd->next = freelist;
  freelist = nd;
}

extern pqueue_t pqueue_new() {
  void *q = malloc(sizeof(qrec));
  assert(q);
  memset(q, 0, sizeof(qrec));
  return q;
}

extern void pqueue_enqueue(pqueue_t q, int prio, void *data) {
  qrec *qr = (qrec *)q;
  node *nd = alloc_node(prio, data);
  node *tmp;

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
}

extern void *pqueue_dequeue(pqueue_t q) {
  qrec *qr = (qrec *)q;
  void *data = NULL;
  node *tmp;

  if (qr->head) {
    tmp = qr->head;
    qr->head = qr->head->next;
    data = tmp->data;
    free_node(tmp);
  }
  return data;
}

extern void *pqueue_peek(pqueue_t q, int *prio) {
  qrec *qr = (qrec *)q;
  void *data = NULL;

  if (qr->head) {
    data = qr->head->data;
    if (prio) {
      *prio = qr->head->prio;
    }
  }
  return data;
}
