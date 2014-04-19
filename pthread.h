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

int updatenode(int id,ucontext_t *updatecontext);

int findactive(void);
void clearactive(void);
void printll();

struct blockedqueue{
	int id;
	struct blockedqueue * next;
};

static int thread_count=0;
struct readyqueue{

    int id;
    struct readyqueue * next;
};

static struct readyqueue * queuehead;
static struct blockedqueue * blockedqueuehead;

struct readyqueue * readyqfrontp;
struct readyqueue * readyqrearp ;

struct blockedqueue * blockedqfrontp ;
struct blockedqueue * blockedqrearp ;

void pushtoreadyqueue(int id);

int popfromreadyqueue();

int pushintoblockedqueue(int id);

void deletefromblockedqueue(int id);

int findinblockedqueue(int id);

pthread_t pthread_create(void(*start_routine)(void*), void *args);
void pthread_yield(void);
int pthread_join(pthread_t thread);
void pthread_exit(void);

#endif
