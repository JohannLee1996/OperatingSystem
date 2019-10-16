#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <pthread.h>
#include "utils.h"

extern void error(char *err) {
  fprintf(stderr, "%s : %s\n", err, strerror(errno));
  abort();
}

extern void lock(pthread_mutex_t *lk) {
  if (pthread_mutex_lock(lk)) {
    error( "Failure to acquire lock!" );
  }
}

extern void unlock(pthread_mutex_t *lk) {
  if (pthread_mutex_unlock(lk)) {
    error( "Failure to acquire lock!" );
  }
}

