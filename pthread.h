#ifndef PTHREAD
#define PTHREAD

#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned long int pthread_t;

struct Threadlist{
 int id;
 int pid;
 int child_count;
 int exit_status;
 int wait_status;
 int active;
 int join;
 int join_count;
 ucontext_t context;
 struct Threadlist * next;
};

struct Threadlist * Threadlisthead;

int addchild(struct Threadlist child);
void deletechild(int id);

struct Threadlist* findnode(int id);

int updateNode(int id,ucontext_t *updatecontext);

int findActive(void);

struct blockedQueue{
	int id;
	struct blockedQueue * next;
};

static int thread_count=0;
struct readyQueue{
  int id;
  struct readyQueue * next;
};

static struct readyQueue * queueHead;
static struct blockedQueue * blockedQueueHead;

struct readyQueue * readyQueueFront;
struct readyQueue * readyQueueRear ;

struct blockedQueue * blockedQueueFront ;
struct blockedQueue * blockedQueueRear ;

void pushToReadyQueue(int id);

int popReadyQueue();

int pushBlockedQueue(int id);

void removeFromBlockedQueue(int id);

int findFromBlockedQueue(int id);

pthread_t pthread_create(void(*start_routine)(void*), void *args);
void pthread_yield(void);
int pthread_join(pthread_t thread);
void pthread_exit(void);

#endif

