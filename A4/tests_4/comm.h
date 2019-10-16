#ifndef AIRPORT_H
#define AIRPORT_H

typedef void *my_msg_t;

extern int comm_allocate();
extern int comm_send(int id, void *buf);
extern void * comm_recv_any(int id);
extern int comm_size(int id);

#endif