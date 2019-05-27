#include "pqueue.h"
#include "pqueue.c"
#include "airport.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

airport *newairport(char name[])
{
    airport *air = (airport *)malloc(sizeof(airport));
    strcpy(air->code, name);
    air->flag = 0;
    air->takingoffQueue = newqueue();
    air->landingQueue = newqueue();
    air->enrouteQueue = newqueue();
    air->waitingQueue = newqueue();
    air->next = NULL;

    return air;
}

airqueue *newairqueue()
{
    airqueue *aq = (airqueue *)malloc(sizeof(airqueue));
    aq->count = 0;
    aq->front = NULL;
    aq->rear = NULL;

    return aq;
}

int airisempty(airqueue *aq)
{
    return (aq->count == 0);
}

void airenqueue(airqueue *aq, airport *a)
{
    if (!airisempty(aq))
    {
        aq->rear->next = a;
        aq->rear = a;
    }
    else
    {
        aq->front = aq->rear = a;
    }
    aq->count++;
}

airport *airdequeue(airqueue *aq)
{
    airport *temp;
    temp = aq->front;
    if (aq->count == 1)
    {
        aq->front = NULL;
        aq->rear = NULL;
    }
    else
    {
        aq->front = aq->front->next;
    }
    aq->count--;

    return temp;
}

airport *find(airqueue *aq, char name[])
{
    airport *temp;
    temp = aq->front;
    while (temp != NULL)
    {
        if (!strcmp(temp->code, name))
        {
            return temp;
        }
        else
        {
            temp = temp->next;
        }
    }
    return NULL;
}

void add_airports(airqueue *aqptr, flight *tempf)
{
    if (find(aqptr, tempf->O) == NULL)
    {
        airport *tempa = newairport(tempf->O);
        airenqueue(aqptr, tempa);
    }
    if (find(aqptr, tempf->D) == NULL)
    {
        airport *tempa = newairport(tempf->D);
        airenqueue(aqptr, tempa);
    }
}

void add_flight_to_depart(airqueue *aqptr, queue *qptr)
{
    airport *tempa;
    flight *tempf;
    while (qptr->front != NULL)
    {
        tempf = dequeue(qptr);
        tempa = find(aqptr, tempf->O);
        enqueue(tempa->waitingQueue, tempf, 1);
    }
}

void sim_take_off(int t, airqueue *aqptr)
{
    airport *ap = aqptr->front;
    while (ap != NULL)
    {
        flight *tempf = ap->waitingQueue->front;
        while (tempf != NULL)
        {
            if (tempf->time == t-10)
            {
                enqueue(ap->takingoffQueue, dequeue(ap->waitingQueue), 1);
                tempf = ap->waitingQueue->front;
            }
            else
                break;
        }
        ap = ap->next;
    }
}

void sim_landing(int t, airqueue *aqptr)
{
    airport *ap = aqptr->front;
    airport *desap;
    while (ap != NULL)
    {
        flight *tempf = ap->enrouteQueue->front;
        while (tempf != NULL)
        {
            if (tempf->estimate_time == t)
            {
                desap = find(aqptr, tempf->D);
                enqueue(desap->landingQueue, dequeue(ap->enrouteQueue), 0);
                tempf = ap->enrouteQueue->front;
            }
            else
                break;
        }
        ap = ap->next;
    }
}

void sim_enroute(int t, airport *ap)
{
    if (ap->takingoffQueue->count != 0)
    {
        flight *tempf = dequeue(ap->takingoffQueue);
        tempf->depart_time = t;
        tempf->estimate_time = tempf->depart_time + tempf->T;
        enqueue(ap->enrouteQueue, tempf, 0);
    }
}

void sim_output(int t, airport *ap, queue *qptr)
{
    flight *tempf = dequeue(ap->landingQueue);
    tempf->landing_time = t+10;
    tempf->delay = tempf->landing_time - tempf->time - tempf->T -20;
    enqueue(qptr, tempf, 2);
    //printf("[%02d:%02d] %s %d from %s to %s, departed %02d:%02d, delay %d.\n", h, m, tempf->C, tempf->N, tempf->O, tempf->D, tempf->H, tempf->M, tempf->delay);
}

void take_turns(int t, airqueue *aqptr, queue *qptr)
{
    airport *ap = aqptr->front;
    while (ap != NULL)
    {
        if (ap->takingoffQueue->count * ap->landingQueue->count)
        {
            if (ap->flag == 1 || ap->flag == 0)
            {
                sim_output(t, ap, qptr);
                ap->flag = 2;
            }
            else if (ap->flag == 2)
            {
                sim_enroute(t, ap);
                ap->flag = 1;
            }
        }
        else if (ap->takingoffQueue->count)
        {
            sim_enroute(t, ap);
            ap->flag = 1;
        }
        else if (ap->landingQueue->count)
        {
            sim_output(t, ap, qptr);
            ap->flag = 2;
        }
        else
        {
            //do nothing
        }
        ap = ap->next;
    }
}