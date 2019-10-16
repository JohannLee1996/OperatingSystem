#ifndef COMM_H
#define COMM_H
#include "pqueue.h"

typedef void *my_msg_t;

typedef struct my_msg my_msg_r;
struct my_msg
{
    int from;
    int to;
    int time;
    int prio;
};

typedef struct my_resp my_resp_r;
struct my_resp
{
    int grant;
};

typedef struct mail_box mail_box_r;
struct mail_box
{
    mail_box_r *next;
    int id;
    pqueue_t *msg_queue;
};

extern int comm_allocate();
extern int comm_send(int id, void *buf);
extern void *comm_recv_any(int id);
extern int comm_size(int id);

#endif