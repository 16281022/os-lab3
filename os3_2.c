#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
 
int Count = 10000;
int temp;
 
void *sale(void *arg) {
	for (int i = 0; i < 10000; i++){
		temp = Count;
		pthread_yield();
		temp = temp - 1;
		pthread_yield();
		Count = temp;
	}
}
 
void *refund(void *arg) {
 
	for (int i = 0; i < 10000 ; i++){
		temp = Count;
		pthread_yield();
		temp = temp + 1;
		pthread_yield();
		Count = temp;
	}
 
}
 
int main(int argc, char *argv[])
 
{
 
	pthread_t p1, p2;
	pthread_create(&p1, NULL, sale, NULL);
	pthread_create(&p2, NULL, refund, NULL);
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
        printf("%d\n",Count);
	return 0;
}

