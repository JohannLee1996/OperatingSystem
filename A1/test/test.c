#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE 1000

struct flight{
  char C[3];
  int N;
  int P;
  char O[4];
  int H;
  int M;
  int T;
  char D[5];
  struct flight *next;
};
typedef struct flight flight;

struct queue{
  int count;
  flight *front;
  flight *rear;
};
typedef struct queue queue;

flight * loadFlight(char s[]);
void init(queue *q);
int isempty(queue *q);
void enqueue(queue *q, char s[]);
void dequeue(queue *q);
flight * peek(queue *q);

int main (int argc, char *argv[]){
  FILE *fp;
  char strLine[30];
  queue q;
  queue * qptr;
  qptr = &q;
  if((fp=fopen(argv[1],"r"))==NULL){
    perror("fopen");
    exit(0);
  }
  init(qptr);

  while (!feof(fp))		//循环读取每一行，直到文件尾
	{ 
		fgets(strLine,30,fp);	//将fp所指向的文件一行内容读到strLine缓冲区
		if(strcmp(strLine,"end")){
      enqueue(qptr,strLine);
    } 
	}

  fclose(fp);

  flight * pf;
  pf = peek(qptr);
  dequeue(qptr);

  return 0;
}

void init(queue *q){
  q->count = 0;
  q->front = NULL;
  q->rear = NULL;
}

int isempty(queue *q){
  return (q->count == 0);
}

void enqueue(queue *q, char s[]){
  flight *f = (flight *)malloc(sizeof(flight));
  char * pch;

  pch = strtok (s," :");
  strcpy(f->C,pch);
  pch = strtok (NULL," :");
  f->N = atoi(pch);
  pch = strtok (NULL," :");
  f->P = atoi(pch);
  pch = strtok (NULL," :");
  strcpy(f->O,pch);
  pch = strtok (NULL," :");
  f->H = atoi(pch);
  pch = strtok (NULL," :");
  f->M = atoi(pch);
  pch = strtok (NULL," :");
  f->T = atoi(pch);
  pch = strtok (NULL," :");
  strcpy(f->D, pch);


  f->next = NULL;
  
  if(!isempty(q)){
    q->rear->next = f;
    q->rear = f;
  }
  else{
    q->front = q->rear = f;
  }
  q->count++;
}

void dequeue(queue *q){
  q->front = q->front->next;
  q->count--;
}

flight * peek(queue *q){
  return q->front;
}

flight * loadFlight(char s[]){
  char * pch;
  flight f;
  flight * fp = (flight *)malloc(sizeof(flight));
  fp = &f;

  pch = strtok (s," :");
  strcpy(f.C,pch);
  pch = strtok (NULL," :");
  f.N = atoi(pch);
  pch = strtok (NULL," :");
  f.P = atoi(pch);
  pch = strtok (NULL," :");
  strcpy(f.O,pch);
  pch = strtok (NULL," :");
  f.H = atoi(pch);
  pch = strtok (NULL," :");
  f.M = atoi(pch);
  pch = strtok (NULL," :");
  f.T = atoi(pch);
  pch = strtok (NULL," :");
  strcpy(f.D, pch);

  return fp;
}
