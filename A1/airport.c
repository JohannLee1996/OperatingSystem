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
        enqueue(tempa->waitingQueue, tempf);
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
            if (tempf->time == t)
            {
                enqueue(ap->takingoffQueue, dequeue(ap->waitingQueue));
            }
            else
                break;
            tempf = ap->waitingQueue->front;
        }
        ap = ap->next;
    }
}

void sim_landing(int t, airqueue *aqptr)
{
    airport *ap = aqptr->front;
    while (ap != NULL)
    {
        flight *tempf = ap->enrouteQueue->front;
        while (tempf != NULL)
        {
            if (tempf->time == t)
            {
                enqueue(ap->landingQueue, dequeue(ap->enrouteQueue));
            }
            else
                break;
            tempf = ap->enrouteQueue->front;
        }
        ap = ap->next;
    }
}

void sim_enroute(int t, airqueue *aqptr){

}