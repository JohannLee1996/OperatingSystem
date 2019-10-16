#ifndef UTILS_H
#define UTILS_H

extern void error(char *s);
extern void lock(pthread_mutex_t *lk);
extern void unlock(pthread_mutex_t *lk);

#endif
