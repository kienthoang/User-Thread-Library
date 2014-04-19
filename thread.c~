/**
 * CS 406: Operating Systems
 * Project 3
 * Kien & Raymond
*/

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#include <ucontext.h>
#include "threadlink.h"

typedef void * Pthread;

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

/////////////////////
//
// ready queue methods
//
/////////////////////

struct readyqueue{

    int id;
    struct readyqueue * next;
};

/* The Pointer to the head of the ready queue */
static struct readyqueue * queuehead=NULL ;

/* The Pointer to the head of the blockedqueue */
static struct blockedqueue * blockedqueuehead=NULL ;

struct readyqueue * readyqfrontp =NULL ;
struct readyqueue * readyqrearp = NULL ;

struct blockedqueue * blockedqfrontp =NULL ;
struct blockedqueue * blockedqrearp = NULL ;

void pushtoreadyqueue(int id){
	struct readyqueue * temp;
	temp=(struct readyqueue*)malloc(sizeof(struct readyqueue));
	temp->id=id;
	temp->next=NULL;

	if(readyqfrontp ==NULL && readyqrearp ==NULL){
		readyqfrontp=temp;
		readyqrearp=temp;
	}
	else{
		readyqrearp->next=temp;
		readyqrearp=temp;
	}
}

int popfromreadyqueue(){

int temp;
	if(readyqfrontp==NULL && readyqrearp==NULL){
		// List is empty
		return -1;
	}
	else if(readyqfrontp==readyqrearp){
		temp=readyqfrontp->id;
		readyqfrontp=NULL;
		readyqrearp=NULL;
		return temp;
	}
	else{
		temp=readyqfrontp->id;
		readyqfrontp=readyqfrontp->next;
		return temp;
	}

}

///////////////////////
//
// blocked list
//////////////////////


int pushintoblockedqueue(int id){

	struct blockedqueue * tmp;
	struct blockedqueue * nextelement;

	if(blockedqueuehead==NULL){
		blockedqueuehead=(struct blockedqueue *)malloc(sizeof(struct blockedqueue));
		blockedqueuehead->id=id;
		blockedqueuehead->next=NULL;
		return 1;
	}

	else {
		tmp=blockedqueuehead;
		nextelement=(struct blockedqueue *)malloc(sizeof(struct blockedqueue));
		nextelement->id=id;
		nextelement->next=NULL;
		while(tmp->next!=NULL){
			tmp=tmp->next;
		}
		tmp->next=nextelement;
		return 1;
	}
	
	return -1;
}

void deletefromblockedqueue(int id){
	struct blockedqueue * tmp=blockedqueuehead;
	
	if(tmp!=NULL){
		if(tmp->id==id){
			blockedqueuehead=blockedqueuehead->next;
		} else {
			while(tmp->next!=NULL) {
				if(tmp->next->id==id) {
					tmp->next=tmp->next->next;
					return;
				} else {
					tmp=tmp->next;
				}
			}
		}
	}
}

int findinblockedqueue(int id){
struct blockedqueue * tmp=blockedqueuehead;

	if(tmp==NULL){
		return -1;
	}

	else {
		while (tmp!=NULL) {
			if(tmp->id==id) {
				return 1;
			} else {
			tmp=tmp->next;
			}
		}
		return -1;
	}
}

////////////////////////
//
//My Pthread functions
///////////////////////

Pthread pthread_create((void *)void(*__start_routine)(void *), void *args)
{
	//Threadlist
	struct Threadlist * parent,* returnid;
	struct Threadlist child;
	ucontext_t childcontext;
	int result;
	parent=findnode(findactive());
	getcontext(&childcontext);
	childcontext.uc_link=0;
	childcontext.uc_stack.ss_sp = malloc (8*1064);
	if(childcontext.uc_stack.ss_sp==NULL){
		printf("\n Cannot allocate memory Program terminating \n");
		exit(0);
	}
	childcontext.uc_stack.ss_size = 8*1064;
	childcontext.uc_stack.ss_flags = 0;
	makecontext(&childcontext, (void(*)(void))__start_routine, 1, args);

	child.id=++thread_count;
	child.child_count=0;
	child.active=0;
	child.join=0;
	child.join_count=0;
	child.wait_status=0;
	child.context=childcontext;
	child.pid=parent->id;


	//add child to linkled info
	result=addchild(child);
	if(result!=-1){
		//push to ready queue
		pushtoreadyqueue(child.id);
	}
		    
	returnid=findnode(child.id);
	return (void *)&returnid->id;
}

// Yield invoking thread
 void pthread_yield(void) {
	int id;
	ucontext_t savecontext,newcontext;
	struct Threadlist * current_thread;
	struct Threadlist * next_thread;

	id=findactive();
	current_thread=findnode(id);

	//pop from ready queue
	pushtoreadyqueue(id);

	id=popfromreadyqueue();
	if(id!=-1){
		//set active to 1
		next_thread=findnode(id);

		current_thread->active=0;
		next_thread->active=1;

		//send old context back to queue
		getcontext(&savecontext);
		current_thread->context=savecontext;
		newcontext=next_thread->context;

		//swap to new context
		swapcontext(&current_thread->context,&next_thread->context);
	}
	return;
}


// Join with a child thread
 int pthread_join(Pthread thread)
{
	int next_id;
	int *tid=(int *)thread;
	int id=*tid;
	struct Threadlist * child,*active,*next_active;
	child=findnode(id);
	active=findnode(findactive());
	if(child==NULL){

		return -1;
	} else if(child->pid==active->id){
		if(active->join_count==0) {
			active->wait_status=1;
		}

		active->join_count=active->join_count+1;
		child->join=1;
		pushintoblockedqueue(active->id);
		active->active=0;
		next_id=popfromreadyqueue();

		/// check for deadlock if no process in ready queue
		next_active=findnode(next_id);
		next_active->active=1;
		swapcontext(&active->context,&next_active->context);
	
		return 1;
	} else if(child->pid!=active->id) {
		return -1;
	} else if(child->id==active->id) {
		printf("\nDeadlock condition\n");
		return -1;
	}
}

void pthread_exit(void) {
	int id,parent_id,child_id,deleteid,grandparent;
	struct Threadlist * parent, *child,*zombies;
	ucontext_t *temp;
	ucontext_t parentcontext,tempcontext;
	struct readyqueue *temp1=readyqfrontp;

	child=findnode(findactive());
	temp1=readyqfrontp;

	//identify parent
	parent_id=child->pid;
	child_id=child->id;
	parent=findnode(parent_id);

	//decrement parent child count

	child->active=0;

	if(parent->wait_status==1){
		if(child->join==1){
			parent->join_count=parent->join_count-1;
		}
		
		if(parent->join_count==0) {
			parent->wait_status=0;
		}
		
		if(parent->wait_status==0&&parent->exit_status==0) {
			pushtoreadyqueue(parent->id);
		}
	}
		
	if(child->child_count>0){
		child->exit_status=1;
	}
	
	if(child->child_count==0){
		parent->child_count=(parent->child_count)-1;
		
		if(parent->child_count==0&&parent->exit_status==1) {
			deletezombies(parent->id);
		} else {
			deletechild(child->id);
		}
	}

	id=popfromreadyqueue();
	if(id==-1){
		parent=Threadlisthead;
		parent->active=1;
		free(child);
		swapcontext(&child->context,&parent->context);
	}
	
	if(id!=-1) {
		parent=findnode(id);
		parent->active=1;
		//free(child);
		swapcontext(&tempcontext,&parent->context);
	}
}

