#ifndef AIRPORT_H
#define AIRPORT_H


typedef void *airport_t;
typedef struct flight_ flight_t;
struct flight_ {
   flight_t  *next;
   char       airline[20];
   int        f_no;
   int        pid;
   airport_t *origin;
   int        departure;
   int        length;
   airport_t *destination;
   int        completed;
};

extern airport_t airport_get(const char *name);
extern int airport_num();
extern void airport_schedule(flight_t *f);
extern airport_t airport_next(airport_t a);
extern flight_t *airport_step(airport_t a, int time);
extern char *airport_name(airport_t a);

#endif


