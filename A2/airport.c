#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "airport.h"
#include "atcprint.h"
#include "pqueue.h"

#define TIME2PRIO(t, f) ((t)*10000 + f->f_no)
#define PRIO2TIME(p) ((p) / 10000)
#define TAXI_TIME 10
#define GROOM_TIME 30

typedef struct airport airport_rec;
struct airport
{
  airport_rec *next;
  char name[20];
  pqueue_t *scheduled;
  pqueue_t *takeoff;
  pqueue_t *enroute;
  pqueue_t *landing;
  pqueue_t *landed;
  pqueue_t *blocking;
  int takeoff_next;
};

static airport_rec *airports;
static plane_t *plane_table;

static plane_t *find_plane(int pid)
{
  plane_t *p;
  for (p = plane_table; p; p = p->next)
  {
    if (p->pid == pid)
    {
      return p;
    }
  }
  return NULL;
}

static void add_plane(int pid)
{
  plane_t *p;
  p = find_plane(pid);
  if (!p)
  {
    p = malloc(sizeof(plane_t));
    assert(p);
    memset(p, 0, sizeof(plane_t));
    p->pid = pid;
    p->status = 0;
    if (plane_table)
    {
      p->next = plane_table;
      plane_table = p;
    }
    else
    {
      plane_table = p;
      p->next = NULL;
    }
  }
}

static void set_status(int pid, int status)
{
  plane_t *p;
  add_plane(pid);
  p = find_plane(pid);
  p->status = status;
}

extern int status_check(int pid)
{
  plane_t *p;
  add_plane(pid);
  p = find_plane(pid);
  return p->status;
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
    a->scheduled = pqueue_new();
    a->takeoff = pqueue_new();
    a->enroute = pqueue_new();
    a->landing = pqueue_new();
    a->landed = pqueue_new();
    a->blocking = pqueue_new();
    a->takeoff_next = 0;
    a->next = airports;
    airports = a;
  }
  return a;
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

static void pump_departures(airport_rec *apt, int time)
{
  flight_t *f;

  while (ready(apt->scheduled, time))
  {
    f = pqueue_dequeue(apt->scheduled);
    if (status_check(f->pid) == 1)
      pqueue_enqueue(apt->blocking, TIME2PRIO(time, f), f);
    else if (status_check(f->pid) == 0)
    {
      pqueue_enqueue(apt->takeoff, TIME2PRIO(time + TAXI_TIME, f), f);
      set_status(f->pid, 1);
    }
  }

  for (f = ready(apt->blocking, time); f; f = f->next)
  {
    if (status_check(f->pid) == 0)
    {
      flight_t *t = pqueue_dequeue(apt->blocking);
      set_status(t->pid, 1);
      pqueue_enqueue(apt->takeoff, TIME2PRIO(time + TAXI_TIME, t), t);
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
  flight_t *temp;
  flight_t *complete = NULL;
  int prio;
  assert(orig);

  if (ready(orig->landed, time))
  {
    temp = pqueue_dequeue(orig->landed);
    set_status(temp->pid, 0);
  }

  pump_departures(orig, time);
  pump_arrivals(orig, time);

  incoming = ready(orig->landing, time);
  outgoing = ready(orig->takeoff, time);

  if (outgoing && (orig->takeoff_next || !incoming))
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
    prio = TIME2PRIO(time + TAXI_TIME + GROOM_TIME, incoming);
    flight_t *temp2 = malloc(sizeof(flight_t));
    temp2->pid = incoming->pid;
    //temp2->f_no = incoming->f_no;
    pqueue_enqueue(orig->landed, prio, temp2);
    orig->takeoff_next = 1;
    complete = incoming;
    incoming->completed = time + TAXI_TIME;
  }

  return complete;
}

extern char *airport_name(airport_t apt)
{
  airport_rec *a = (airport_rec *)apt;
  assert(a);
  return a->name;
}
