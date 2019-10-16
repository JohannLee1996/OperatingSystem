#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "comm.h"
#include "pqueue.h"
#include "utils.h"

static mail_box_r *mail_box_queue;

static mail_box_r *find_mail_box(int id)//find mail box by id
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

extern int comm_allocate()//allocate a mail box and generate an id
{                           //the ids are from 1 to n
    mail_box_r *new_box;
    new_box = malloc(sizeof(mail_box_r));
    if (!mail_box_queue)//when there is no mail box in the queue, give id 1
    {
        new_box->id = 1;
        new_box->msg_queue = pqueue_new();
        new_box->next = NULL;
        mail_box_queue = new_box;
        return new_box->id;
    }
    else//when there are mail boxs in the queue, give succissive id
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

extern int comm_send(int id, void *buf)//used to send msg
{
    mail_box_r *mb;
    mb = find_mail_box(id);
    if (mb)
    {
        int prio;
        prio = ((my_msg_r *)buf)->prio;//use prio to send msg
        pqueue_enqueue(mb->msg_queue, prio, buf);
    }
    else
    {
        printf("fialed to send mail!\n");
    }
    return 0;
}

extern void *comm_recv_any(int id)//check if there is any new msg in inbox, and 
{                                   //return the first msg
    mail_box_r *mb;
    mb = find_mail_box(id);
    if (mb)
    {
        return pqueue_dequeue(mb->msg_queue);
    }
    return NULL;
}

extern int comm_size(int id)// get the size of the inbox
{
    mail_box_r *mb;
    mb = find_mail_box(id);
    if (mb)
    {
        return pqueue_size(mb->msg_queue);// I wrote a new method to get the size of queue
    }
    return 0;
}