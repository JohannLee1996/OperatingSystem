#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pqueue.h"

queue *newqueue()
{
    queue *q = (queue *)malloc(sizeof(queue));
    q->count = 0;
    q->front = NULL;
    q->rear = NULL;

    return q;
}

int isempty(queue *q)
{
    return (q->count == 0);
}

void insertbefore(queue *q, flight *f1, flight *f)
{
    if (f1 != q->front)
    {
        f1->last->next = f;
        f->last = f1->last;
        f1->last = f;
        f->next = f1;
    }
    else
    {
        f->next = q->front;
        q->front->last = f;
        q->front = f;
        f->last = NULL;
    }
}

void insertafter(queue *q, flight *f1, flight *f)
{
    if (f1!=q->rear)
    {
        f1->next->last = f;
        f->next = f1->next;
        f1->next = f;
        f->last = f1;
    }
    else
    {
        f->last = q->rear;
        q->rear->next = f;
        q->rear = f;
        f->next = NULL;
    }
}

void enqueue(queue *q, flight *f, int istakingoff)
{
    if (!isempty(q))
    {
        int tf = 0;
        int tt = 0;
        flight *temp = q->front;

        while (temp != NULL)
        {
            if(istakingoff){
            tf = f->time;
            tt = temp->time;
        }
        else{
            tf = f->estimate_time;
            tt = temp->estimate_time;
        }
        
            if (tf < tt)
            {
                insertbefore(q, temp, f);
                break;
            }
            else if (tf > tt)
            {
                if (temp->next == NULL)
                {
                    insertafter(q, temp, f);
                    break;
                }
            }
            else
            {
                if (f->N < temp->N)
                {
                    insertbefore(q, temp, f);
                    break;
                }
            }
            if (temp->next == NULL)
            {
                insertafter(q, temp, f);
                break;
            }
            else
            {
                temp = temp->next;
            }
        }
    }
    else
    {
        q->front = q->rear = f;
        f->last = NULL;
    }
    q->count++;
}

flight *dequeue(queue *q)
{
    flight *temp;
    temp = q->front;
    if (q->front == NULL)
    {
        return NULL;
    }
    if (q->front->next != NULL)
    {
        q->front = q->front->next;
        q->front->last = NULL;
    }
    else
    {
        q->rear = q->front = NULL;
    }
    q->count--;
    temp->next = NULL;
    temp->last = NULL;

    return temp;
}

flight *peek(queue *q)
{
    return q->front;
}

flight *loadFlight(char s[])
{
    char *pch;
    flight *f = (flight *)malloc(sizeof(flight));

    pch = strtok(s, " :");
    strcpy(f->C, pch);
    pch = strtok(NULL, " :");
    f->N = atoi(pch);
    pch = strtok(NULL, " :");
    f->P = atoi(pch);
    pch = strtok(NULL, " :");
    strcpy(f->O, pch);
    pch = strtok(NULL, " :");
    f->H = atoi(pch);
    pch = strtok(NULL, " :");
    f->M = atoi(pch);
    pch = strtok(NULL, " :");
    f->T = atoi(pch);
    pch = strtok(NULL, " :\n");
    strcpy(f->D, pch);
    f->time = f->H * 60 + f->M;
    f->depart_time = 0;
    f->estimate_time = 0;
    f->landing_time = 0;
    f->delay = 0;

    f->last = NULL;
    f->next = NULL;

    return f;
}
