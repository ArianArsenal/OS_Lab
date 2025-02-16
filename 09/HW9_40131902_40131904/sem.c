#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

int shared_mem= 1;
sem_t s;

void* f1(){
	int x;
    sem_post(&s);
	sem_wait(&s);
	x= shared_mem;
	printf("f1, %d\n", x);
	x++;
	printf("f1 updated\n");
	sleep(1);
	shared_mem= x;
	printf("new val f1: %d\n", shared_mem);
	sem_post(&s);
}
void* f2(){
	int x;
	// sem_post(&s);
	sem_wait(&s);
	x= shared_mem;
	printf("f2, %d\n", x);
	x--;
	printf("f2 updated\n");
	sleep(1);
	shared_mem= x;
	printf("new val f2: %d\n", shared_mem);
	sem_post(&s);
}

int main(){
	pthread_t t1, t2;
	pthread_create(&t2, NULL, f2, NULL);
	pthread_create(&t1, NULL, f1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t1, NULL);
	printf("final: %d\n", shared_mem);

}
