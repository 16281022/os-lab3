#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <time.h>
sem_t *p1,*p2,*p3,*p4;

int main(int argc, char *argv[])
{
p1 = sem_open("p1",O_CREAT,0666,0);
p2 = sem_open("p2",O_CREAT,0666,0);
p3 = sem_open("p3",O_CREAT,0666,0);
p4 = sem_open("p4",O_CREAT,0666,1);
if (fork() == 0)
{
printf("I am the proccess P1\n");
sem_post(p1);
sem_post(p1);
}
else
{
if (fork() == 0)
{
sem_wait(p1);
sem_wait(p4);
printf("I am the proccess P2\n");
sem_post(p4);
sem_post(p2);
}
else
{
if (fork() == 0)
{
sem_wait(p1);
sem_wait(p4);
printf("I am the proccess P3\n");
sem_post(p4);
sem_post(p3);
}
else
{
if (fork() == 0)
{
sem_wait(p2);
sem_wait(p3);
printf("I am the proccess P4\n");
}
}
}
}
sem_close(p1);
sem_close(p2);
sem_close(p3);
sem_close(p4);

sem_unlink("p1");
sem_unlink("p2");
sem_unlink("p3");
sem_unlink("p4");
return 0;
}

