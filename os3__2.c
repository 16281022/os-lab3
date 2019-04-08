#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
 
int Count = 10000;
int temp;
 
sem_t *s1,*s2,*s3;
 
void *sale(void *arg) {
	for (int i = 0; i < 10000; i++){
		sem_wait(&s1);
                sem_wait(&s3);
		temp = Count;
		pthread_yield();
		temp = temp - 1;
		pthread_yield();
		Count = temp;
		sem_post(s3);
                sem_post(s2);
	}
}
 
void *refund(void *arg) {
 
	for (int i = 0; i < 10000; i++){
		sem_wait(s2);
                sem_wait(s3);
		temp = Count;
		pthread_yield();
		temp = temp + 1;
		pthread_yield();
		Count = temp;
		sem_post(s3);
                sem_post(s1);
	}
 
}
 
int main(int argc, char *argv[])
 
{	
        s1 = sem_open("s1",O_CREAT,0666,10000);
        s2 = sem_open("s2",O_CREAT,0666,0);
 	s3 = sem_open("s3",O_CREAT,0666,1);
	pthread_t p1, p2;
	pthread_create(&p1, NULL, sale, NULL);
	pthread_create(&p2, NULL, refund, NULL);
 
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
 
	sem_close(s1);
        sem_close(s2); 
        sem_close(s3);
        sem_unlink("s1");
        sem_unlink("s2");
        sem_unlink("s3");
        printf("%d\n",Count);
 
	return 0;
}

