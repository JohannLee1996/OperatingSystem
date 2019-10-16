#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "comm.h"
#include "pqueue.h"

typedef struct my_msg my_msg_r;
struct my_msg
{
    char message[20];
    int from;
    int to;
    int time;
};

typedef struct my_resp my_resp_r;
struct my_resp{
    int grant;
};

typedef struct mail_box mail_box_r;
struct mail_box
{
    mail_box_r *next;
    int id;
    pqueue_t *msg_queue;
};

static mail_box_r *mail_box_queue;

extern int if_time_duplicate(pqueue_t q, int time)
{
    node *nd;
    my_msg_r *msg;
    qrec *qr = (qrec *)q;
    for (nd = qr->head; nd; nd = nd->next)
    {
        msg = (my_msg_r *)(nd->data);
        if (time == msg->time)
        {
            return 1;
        }
    }
    return 0;
}

static mail_box_r *find_mail_box(int id)
{
    mail_box_r *mb;

    for (mb = mail_box_queue; mb; mb = mb->next)
    {
        if (id == mb->id)
        {
            return mb;
        }
    }
    return NULL;
}

extern int comm_allocate()
{
    mail_box_r *new_box;
    new_box = malloc(sizeof(mail_box_r));
    if (!mail_box_queue)
    {
        new_box->id = 1;
        new_box->msg_queue = pqueue_new();
        new_box->next = NULL;
        mail_box_queue = new_box;
        return new_box->id;
    }
    else
    {
        mail_box_r *temp;
        for (temp = mail_box_queue; temp; temp = temp->next)
        {
            if (!(temp->next))
            {
                new_box->id = temp->id + 1;
                new_box->msg_queue = pqueue_new();
                new_box->next = NULL;
                temp->next = new_box;
                return new_box->id;
            }
        }
        return 0;
    }
}

extern int comm_send(int id, void *buf)
{
    mail_box_r *mb;
    mb = find_mail_box(id);
    if (mb)
    {
        int prio;
        my_msg_r *msg = (my_msg_r *)buf;
        if (if_time_duplicate(mb->msg_queue, msg->time))
        {
            my_resp_r *res;
            res = malloc(sizeof(my_msg_r));
            res->grant = 0;
            pqueue_enqueue(find_mail_box(msg->from), 0, (void *)res);
        }
        else
        {
            pqueue_enqueue(mb->msg_queue, 0, buf);
        }
    }
    else
    {
        printf("fialed to send mail!\n");
    }
    return 0;
}

extern void *comm_recv_any(int id)
{
    mail_box_r *mb;
    mb = find_mail_box(id);
    if (mb)
    {
        return pqueue_dequeue(mb->msg_queue);
    }
    return NULL;
}

extern int comm_size(int id)
{
    mail_box_r *mb;
    mb = find_mail_box(id);
    if (mb)
    {
        return pqueue_size(mb->msg_queue);
    }
    return 0;
}