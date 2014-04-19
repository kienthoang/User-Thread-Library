/**
 * CS 406: Operating Systems
 * Project 3
 * Kien & Raymond
*/

#ifndef PTHREAD
#define PTHREAD

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#include <ucontext.h>

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

struct ZombieQueue{
	int id;
	struct ZombieQueue * next;
};

static struct ZombieQueue * zomebieQueueHead=NULL;
struct ZombieQueue * zombieqfrontp =NULL;
struct ZombieQueue * zombieqrearp = NULL;

void pushToZombieQueue(int id) {
	struct ZombieQueue * temp;
	temp=(struct ZombieQueue*)malloc(sizeof(struct ZombieQueue));
	temp->id=id;
	temp->next=NULL;

	if(zombieqfrontp == NULL && zombieqrearp == NULL){
		zombieqfrontp=temp;
		zombieqrearp=temp;
	} else{
		zombieqrearp->next=temp;
		zombieqrearp=temp;
	}
}

int popZombieQueue() {
	int temp;
	
	if(zombieqfrontp==NULL && zombieqrearp==NULL){
		return -1;
	} else if(zombieqfrontp==zombieqrearp){
		temp=zombieqfrontp->id;
		zombieqfrontp=NULL;
		zombieqrearp=NULL;
		
		return temp;
		
	} else {
		temp=zombieqfrontp->id;
		zombieqfrontp=zombieqfrontp->next;
		return temp;
	}
}

struct Threadlist * Threadlisthead;

int addchild(struct Threadlist child){
	struct Threadlist* tmp,* lastchild;
	
	if(Threadlisthead == NULL){
		Threadlisthead = (struct Threadlist*)malloc(sizeof(struct Threadlist));
		
		if(Threadlisthead==NULL){
			exit(0);
		}
		
		Threadlisthead->id=child.id;
		Threadlisthead->pid=child.pid;
		Threadlisthead->child_count=child.child_count;
		Threadlisthead->active=child.active;
		Threadlisthead->context=child.context;
		Threadlisthead->exit_status=child.exit_status;
		Threadlisthead->wait_status=child.wait_status;
		Threadlisthead->join_count=child.join_count;
		Threadlisthead->join=child.join;
		Threadlisthead->next=NULL;

		return 1;
	} else {
		tmp=Threadlisthead;
		while(tmp->next!=NULL){
			tmp=tmp->next;
		}

		lastchild= (struct Threadlist*)malloc(sizeof(struct Threadlist));
		if(lastchild==NULL){
			exit(0);
		}
		
		tmp->next=lastchild;
		lastchild->id=child.id;
		lastchild->pid=child.pid;
		lastchild->child_count=child.child_count;
		lastchild->active=child.active;
		lastchild->context=child.context;

		lastchild->exit_status=child.exit_status;
		lastchild->wait_status=child.wait_status;
		lastchild->join_count=child.join_count;

		lastchild->join=child.join;
		lastchild->next=NULL;

		return 1;
	}

	return 0;
}

void setchildjoins(int id) {
	struct Threadlist *temp=Threadlisthead;

	if(temp!=NULL){
		while(temp!=NULL){
			if(temp->pid==id){
				temp->join=1;
			}
			
			temp=temp->next;
		}
	}
}

void deletechild(int id){
	struct Threadlist *deletenode = Threadlisthead;
	
	struct Threadlist *delete;
	struct Threadlist *prevnode = NULL;
	
	if(deletenode!=NULL){
		if(deletenode->id==id){
			delete=Threadlisthead;
			Threadlisthead= Threadlisthead->next;
			free(delete);
		} else {
			prevnode=deletenode;
			
			while(deletenode!=NULL){
				if(deletenode->id==id){
					prevnode->next=deletenode->next;
					break;
				} else {
					prevnode=deletenode;
					deletenode=deletenode->next;
				}
			}
		}
	}
}


struct Threadlist * findnode(id){
	struct Threadlist * homenode = Threadlisthead;

	do {
		if(homenode->id==id){
			return homenode;
		} else {
			homenode=homenode->next;
		}
	} while(homenode!=NULL);

	return NULL;
}

void deletezombies(int id){
	struct Threadlist * node, *temp ;

	while(id!=1){
		node=findnode(id);
		if(node->exit_status==1&&node->child_count==0){
			pushToZombieQueue(id);
			id=node->pid;
			temp=findnode(id);
			temp->child_count=temp->child_count-1;
		} else {
			break;
		}
	}

	id=popZombieQueue();
	
	while(id!=-1){
		temp=findnode(id);
		deletechild(id);
		free(temp);
		id=popZombieQueue();
	}
}

int updatenode(int id,ucontext_t updatecontext) {
	struct Threadlist * homenode = Threadlisthead;
	
	do {
		if(homenode->id==id) {
			homenode->context=updatecontext;
			return 1;
		} else {
			homenode=homenode->next;
		}
	} while(homenode!=NULL);

	return -1;
}

int findactive(void) {
	printf("HAHAHA\n");
	struct Threadlist * homenode = Threadlisthead;
	int id;
	printf("HAHAHA\n");
	while(homenode!=NULL){
		printf("HAHAHA0\n");
		id=homenode->id;
		printf("HAHAHA1\n");

		if(homenode->active==1) {
			printf("HAHAHA\n");
			return id;
		} else {
			homenode=homenode->next;
			printf("HAHAHA123\n");
		}
	}

	return -1;
}
void clearactive(void){
	struct Threadlist * homenode = Threadlisthead;
	
	do {
		if(homenode->active==1) {
			homenode->active=0;
			return;
		} else {
			homenode=homenode->next;
		}
	} while(homenode!=NULL);

	return;
}

void printll(){
	struct Threadlist * tmp=Threadlisthead;
	
	if(tmp!=NULL) {
		printf("%d---",tmp->id);
		
		while(tmp->next!=NULL){
			tmp=tmp->next;
			printf(" %d ->",tmp->id);
		}
	}
	else {
		printf("tlist empty");
	}
}

struct blockedqueue{
	int id;
	struct blockedqueue * next;
};

static int thread_count=0;

struct readyqueue{

    int id;
    struct readyqueue * next;
};

static struct readyqueue * queuehead=NULL ;

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
	
	if(readyqfrontp==NULL && readyqrearp==NULL) {
		return -1;
	} else if(readyqfrontp==readyqrearp) {
		temp=readyqfrontp->id;
		readyqfrontp=NULL;
		readyqrearp=NULL;
		return temp;
	} else {
		temp=readyqfrontp->id;
		readyqfrontp=readyqfrontp->next;
		return temp;
	}

}

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

pthread_t pthread_create(void(*start_routine)(void*), void *args) {
	printf("YAYAYAYAYAYYAYA\n");
	struct Threadlist * parent,* returnid;
	struct Threadlist child;
	printf("YAYAYAYAYAYYAYA\n");
	ucontext_t childcontext;
	int result;
	printf("YAYAYAYAYAYYAYA\n");
	parent=findnode(findactive());
	printf("YAYAYAYAYAYYAYA\n");
	getcontext(&childcontext);
	childcontext.uc_link=0;
	childcontext.uc_stack.ss_sp = malloc (8*1064);
	if(childcontext.uc_stack.ss_sp==NULL){
		exit(0);
	}
printf("HIHIHIHI\n");
	childcontext.uc_stack.ss_size = 8*1064;
	childcontext.uc_stack.ss_flags = 0;
	makecontext(&childcontext, (void(*)(void))start_routine, 1, args);

	child.id=++thread_count;
	child.child_count=0;
	child.active=0;
	child.join=0;
	child.join_count=0;
	child.wait_status=0;
	child.context=childcontext;
	child.pid=parent->id;

	result=addchild(child);
	if(result!=-1){
		pushtoreadyqueue(child.id);
	}
		    
	returnid=findnode(child.id);
	return (void *)&returnid->id;
}

void pthread_yield(void) {
	int id;
	ucontext_t savecontext,newcontext;
	struct Threadlist * current_thread;
	struct Threadlist * next_thread;

	id=findactive();
	current_thread=findnode(id);

	pushtoreadyqueue(id);

	id=popfromreadyqueue();
	if(id!=-1){
		next_thread=findnode(id);

		current_thread->active=0;
		next_thread->active=1;

		getcontext(&savecontext);
		current_thread->context=savecontext;
		newcontext=next_thread->context;

		swapcontext(&current_thread->context,&next_thread->context);
	}
	return;
}


 int pthread_join(pthread_t thread)
{
	int next_id;

	struct Threadlist * child,*active,*next_active;
	child = findnode(thread);
	active = findnode(findactive());
	
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

		next_active=findnode(next_id);
		next_active->active=1;
		swapcontext(&active->context,&next_active->context);
	
		return 1;
	} else if(child->pid!=active->id) {
		return -1;
	} else if(child->id==active->id) {
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

	parent_id=child->pid;
	child_id=child->id;
	parent=findnode(parent_id);


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
		swapcontext(&tempcontext,&parent->context);
	}
}

#endif

