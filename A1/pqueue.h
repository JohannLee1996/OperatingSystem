//
//  pqueue.h
//  JinfengLi
//
//  Created by LiJunhong on 2019-05-16.
//  Copyright © 2019 LiJunhong. All rights reserved.
//

#ifndef pqueue_h
#define pqueue_h

struct flight{
  char C[3];
  int N;
  int P;
  char O[4];
  int H;
  int M;
  int T;
  char D[4];
  int time;
  int depart_time;
  int estimate_time;
  struct flight *last;
  struct flight *next;
};
typedef struct flight flight;

struct queue{
  int count;
  flight *front;
  flight *rear;
};
typedef struct queue queue;

#endif /* pqueue_h */


