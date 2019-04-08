#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
 
char buf[10];
sem_t *s1 , *s2;
int i = 0,j = 0;
 
void *worker1(void *arg) {
 
	while(1) {
        sem_wait(s1);
	scanf("%c",&buf[i]);
	sem_post(s2);
	i = (i+1)%10;
	}
        return NULL;
}
 
void *worker2(void *arg) {
 
	while(1) {
        sem_wait(s2);
	printf("%c\n",buf[j]);
	sem_post(s1);
        sleep(1);
	j = (j+1)%10;
	}
        return NULL;
}
 
 
 
int main(int argc, char *argv[])
 
{	
	s1 = sem_open("s1",O_CREAT,0666,10);
        s2 = sem_open("s2",O_CREAT,0666,0);
        pthread_t p1, p2;
	pthread_create(&p1, NULL, worker1, NULL);
	pthread_create(&p2, NULL, worker2, NULL);
 
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
        sem_close(s1);
        sem_close(s2);
        sem_unlink("s1");
        sem_unlink("s2");
	return 0;
 
}

