#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "airport.h"
#include "atcprint.h"
#include "pqueue.h"
#include "utils.h"
#include "atsim3.h"
#include "comm.h"

#define TIME2PRIO(t, f) ((t)*10000 + f->f_no)
#define PRIO2TIME(p) ((p) / 10000)
#define TAXI_TIME 10
#define GROOM_TIME 30
#define MAX_PID 1000

typedef struct airport airport_rec;
struct airport
{
  pthread_mutex_t record_lock;
  airport_rec *next;
  char name[20];
  pqueue_t *scheduled;
  pqueue_t *takeoff;
  pqueue_t *enroute;
  pqueue_t *landing;
  pqueue_t *grooming;
  flight_t *blocked;
  int inbox;// inbox id
  int response;//response inbox id
  int record[1440];//record free time in a day
  int takeoff_next;
};

static airport_rec IN_USE[1];
static airport_rec *airports;
static airport_rec *plane_loc[MAX_PID];
static int num_airports;

extern void set_inbox(airport_t *temp_ap, int mailbox_id)//used to set inbox id to an airport
{
  airport_rec *ap = (airport_rec *)temp_ap;
  ap->inbox = mailbox_id;
}

extern void set_response(airport_t *temp_ap, int mailbox_id)//used to set response inbox id to an airport
{
  airport_rec *ap = (airport_rec *)temp_ap;
  ap->response = mailbox_id;
}

static airport_rec *find(const char *name)
{
  airport_rec *a;

  for (a = airports; a; a = a->next)
  {
    if (!strcmp(name, a->name))
    {
      break;
    }
  }
  return a;
}

extern airport_t airport_get(const char *name)
{
  airport_rec *a = find(name);

  assert(name);

  if (!a)
  {
    a = malloc(sizeof(airport_rec));
    assert(a);
    memset(a, 0, sizeof(airport_rec));
    strcpy(a->name, name);
    if (pthread_mutex_init(&a->record_lock, NULL))
    {
      error("Could not initialize mutex lock");
    }
    a->scheduled = pqueue_new();
    a->takeoff = pqueue_new();
    a->enroute = pqueue_new();
    a->landing = pqueue_new();
    a->grooming = pqueue_new();
    a->takeoff_next = 0;
    a->next = airports;
    a->inbox = 0;// initialize to 0
    a->response = 0;// initialize to 0
    for (int i = 0; i < 1440; i++)
    {
      a->record[i] = 0;//initialize all time to 0
    }
    airports = a;
    num_airports++;
  }
  return a;
}

extern int airport_num()
{
  return num_airports;
}

extern void airport_schedule(flight_t *f)
{
  assert(f);
  int prio = TIME2PRIO(f->departure, f);
  pqueue_enqueue(((airport_rec *)(f->origin))->scheduled, prio, f);
}

extern airport_t airport_next(airport_t a)
{
  airport_rec *ar = (airport_rec *)a;

  if (ar)
  {
    return ar->next;
  }
  return airports;
}

static flight_t *ready(pqueue_t q, int time)
{
  int t;
  flight_t *f = pqueue_peek(q, &t);

  if (f && (PRIO2TIME(t) <= time))
  {
    return f;
  }
  return NULL;
}

static void block(airport_rec *apt, flight_t *f)
{
  f->next = apt->blocked;
  apt->blocked = f;
}

static flight_t *unblock(airport_rec *apt, int pid)
{
  flight_t *f = apt->blocked;
  flight_t *tmp;

  if (f)
  {
    if (f->pid == pid)
    {
      apt->blocked = f->next;
      f->next = NULL;
    }
    else
    {
      f = NULL;
      for (tmp = apt->blocked; tmp->next; tmp = tmp->next)
      {
        if (tmp->next->pid == pid)
        {
          f = tmp->next;
          tmp->next = f->next;
          f->next = NULL;
          break;
        }
      }
    }
  }
  return f;
}

static void send_request(flight_t *f, int time)//create msg and send it to an airport
{
  airport_rec *dest;
  airport_rec *orig;
  dest = (airport_rec *)f->destination;
  orig = (airport_rec *)f->origin;
  my_msg_r *new_msg;
  new_msg = malloc(sizeof(my_msg_r));
  new_msg->from = orig->response;
  new_msg->to = dest->inbox;
  new_msg->time = time + f->length;
  new_msg->prio = 1000 * time + f->f_no;//here is the prio
  comm_send(new_msg->to, (void *)new_msg);
}

static void send_response(my_msg_r *in_msg, int grant)//used to response a request
{
  my_resp_r *resp;
  resp = malloc(sizeof(my_resp_r));
  resp->grant = grant;
  comm_send(in_msg->from, (void *)resp);
}

static void pump_departures(airport_rec *apt, int time)
{
  flight_t *f;

  if (ready(apt->scheduled, time))
  {
    f = pqueue_dequeue(apt->scheduled);

    if ((plane_loc[f->pid] != NULL) && (plane_loc[f->pid] != apt))
    {
      block(apt, f);
    }
    else
    {
      pqueue_enqueue(apt->takeoff, TIME2PRIO(time + TAXI_TIME, f), f);
      plane_loc[f->pid] = IN_USE;
    }
  }

  if (ready(apt->grooming, time))
  {
    f = pqueue_dequeue(apt->grooming);
    plane_loc[f->pid] = apt;
    f = unblock(apt, f->pid);
    if (f)
    {
      pqueue_enqueue(apt->takeoff, TIME2PRIO(time + TAXI_TIME, f), f);
      plane_loc[f->pid] = IN_USE;
    }
  }
}

static void pump_arrivals(airport_rec *apt, int time)
{
  flight_t *f;

  while (ready(apt->enroute, time))
  {
    f = pqueue_dequeue(apt->enroute);
    pqueue_enqueue(apt->landing, TIME2PRIO(time, f), f);
  }
}

extern flight_t *airport_step(airport_t apt, int time)
{
  airport_rec *orig = (airport_rec *)apt;
  airport_rec *dest;
  flight_t *incoming;
  flight_t *outgoing;
  flight_t *complete = NULL;
  int prio;
  assert(orig);

  pump_departures(orig, time);
  pump_arrivals(orig, time);

  incoming = ready(orig->landing, time);
  outgoing = ready(orig->takeoff, time);

  if (outgoing)
  {
    send_request(outgoing, time);//before take off, send request to dest airport
  }
  barrier();

  my_msg_r *in_msg = NULL;

  for (in_msg = (my_msg_r *)comm_recv_any(orig->inbox); in_msg; in_msg = (my_msg_r *)comm_recv_any(orig->inbox))
  {// handle every msg in inbox and give response
    pthread_mutex_lock(&orig->record_lock);
    if (orig->record[in_msg->time] == 0)//time slot is free
    {
      send_response(in_msg, 1);
      orig->record[in_msg->time] = 1;
    }
    else//time slot is not free
    {
      send_response(in_msg, 0);
    }
    pthread_mutex_unlock(&orig->record_lock);
  }
  barrier();

  my_resp_r *in_resp = NULL;
  in_resp = (my_resp_r *)comm_recv_any(orig->response);//get response from dest
  int grant = 0;
  if (in_resp)
  {
    if (in_resp->grant)
    {
      grant = 1;//get permission to take off
    }
  }

  if (grant && outgoing && (orig->takeoff_next || !incoming))//only when there is a permission can the plane take off
  {
    pqueue_dequeue(orig->takeoff);
    orig->takeoff_next = 0;
    dest = (airport_rec *)outgoing->destination;
    prio = TIME2PRIO(time + outgoing->length, outgoing);
    pqueue_enqueue(dest->enroute, prio, outgoing);
  }
  else if (incoming)
  {
    pqueue_dequeue(orig->landing);
    orig->takeoff_next = 1;
    complete = incoming;
    incoming->completed = time + TAXI_TIME;
    prio = TIME2PRIO(incoming->completed + GROOM_TIME, incoming);
    pqueue_enqueue(orig->grooming, prio, incoming);
  }

  return complete;
}

extern char *airport_name(airport_t apt)
{
  airport_rec *a = (airport_rec *)apt;
  assert(a);
  return a->name;
}
