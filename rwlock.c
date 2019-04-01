#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM 1000


 struct mystruct {
pthread_mutex_t mylock;
pthread_cond_t read_ok;
pthread_cond_t write_ok;
int r_pend,r_count;
int w_pend,w_count;

};

 struct foo_struct
{
	struct mystruct *lock_ptr;
	int *data;
};

void rwlock_init(struct mystruct *lock)
{

	pthread_cond_init(&(lock->write_ok),NULL);
	pthread_cond_init(&(lock->read_ok),NULL);
	pthread_mutex_init(&(lock->mylock),NULL);
	lock->r_pend = 0;
	lock->r_count = 0;
	lock->w_pend = 0;
	lock->w_count = 0;
}


void readlock(struct mystruct *lock)
{


	pthread_mutex_lock(&(lock->mylock));
	(lock->r_pend)++;
	while(lock->w_count>0 || lock->w_pend>0)
		pthread_cond_wait(&(lock->read_ok),&(lock->mylock));
	(lock->r_pend)--;(lock->r_count)++;
	pthread_mutex_unlock(&(lock->mylock));
}

void writelock(struct mystruct *lock)
{
	
	pthread_mutex_lock(&(lock->mylock));
	(lock->w_pend)++;
	while(lock->w_count>0 || lock->r_count>0)
		pthread_cond_wait(&(lock->write_ok),&(lock->mylock));
	(lock->w_pend)--;
	(lock->w_count)++;
	pthread_mutex_unlock(&(lock->mylock));
}

void unlock(struct mystruct *lock)
{

	pthread_mutex_lock(&(lock->mylock));
	if(lock->w_count>0)lock->w_count=0;
	else if(lock->r_count>0) (lock->r_count)--;
	pthread_mutex_unlock(&(lock->mylock));

	if(lock->r_count==0 && lock->w_pend>0)
		pthread_cond_signal(&(lock->write_ok));
	else if(lock->w_count==0 && lock->r_pend>0)
		pthread_cond_signal(&(lock->read_ok));
}


struct node { int d; struct node *n; } *list;
void insert(int d)
{
       	struct node *x = (struct node *)malloc(sizeof(struct node));
	x->d = d; x->n = list; list = x;
}

struct node* search(int d)
{
	struct node *nd;
	for(nd = list; (nd != NULL)&&(nd->d != d); nd = nd->n);
	return nd;
}




void *foo(void *x)
{
	int id = *(( ((struct foo_struct *) x)->data));
	int i, j = -1;
	for(i = 0; i < 10*NUM; i++)
		if(i%10 == 0)
		{
			writelock( (( ((struct foo_struct *) x)->lock_ptr)) );
			j++; insert (1000*id+j); 
			unlock( (( ((struct foo_struct *) x)->lock_ptr)) );
			
		}
		else
		{
			readlock((( ((struct foo_struct *) x)->lock_ptr)));
			search (1000*id+j);
			unlock( (( ((struct foo_struct *) x)->lock_ptr)) );
		}
	pthread_exit(NULL);
}

int main()
{
	list = NULL;
	
	 struct mystruct *lock = (struct mystruct *)malloc(sizeof(struct mystruct));
	rwlock_init(lock);
	
	int i, a[5];
	pthread_t t[5];
	struct foo_struct foo_s[5];

	for(i = 0; i < 5; i++) {
		foo_s[i].lock_ptr = lock;

		a[i] = i;
		foo_s[i].data = &a[i];
		 pthread_create(&t[i],NULL,foo,&foo_s[i]);}
	for(i = 0; i < 5; i++) pthread_join(t[i],NULL);

		
	int count = 0;
	for(struct node *nd = list; nd != NULL; nd = nd->n) { printf("%d, ",nd->d); count++;}
	puts("");
	if(count == 5*NUM) printf("OK\n"); else printf("ERROR\n");
}
