# os-lab3
## 实验三 同步与通信
### 安全1601 战泓夷 16281022
#### 1.实验目的
* 系统调用的进一步理解。
* 进程上下文切换。
* 同步与通信方法。
#### 2.实验题目
1）通过fork的方式，产生4个进程P1,P2,P3,P4，每个进程打印输出自己的名字，例如P1输出“I am the process P1”。要求P1最先执行，P2、P3互斥执行，P4最后执行。通过多次测试验证实现是否正确。<br>
sem_open:创建并初始化信号量，头文件是<semaphore.h>。<br>
sem_post:挂出共享资源，成功返回0，不成功返回-1。<br>

* 参考代码：<br>
```
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

```
信号量p1代表进程1是否执行结束，为了p2，p3都可以执行，执行结束后需要signal两次；信号量p2代表进程2是否执行结束，执行结束后需要signal一次；信号量p2代表进程3是否执行结束，执行结束后需要signal一次；信号量p4是互斥向量，为了保证p2、p3互斥执行。<br>
执行结果：<br>
![image](https://github.com/16281022/-_16281022/blob/master/os.png)<br><br>
2）火车票余票数ticketCount 初始值为1000，有一个售票线程，一个退票线程，各循环执行多次。添加同步机制，使得结果始终正确。要求多次测试添加同步机制前后的实验效果。<br>
* 不添加同步机制的参考代码:<br>
```
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
```
执行结果:<br>
![image](https://github.com/16281022/-_16281022/blob/master/os.png)<br><br>
初始票数10000，买票10000，退票10000，最终剩余票数10000，我们可以看出结果与正确结果不符合，出现错误。<br>
* 添加同步机制的参考代码:<br>
```
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
```
执行结果:<br>
![image](https://github.com/16281022/-_16281022/blob/master/os3_4.jpg)<br><br>
初始票数10000，买票10000，退票10000，最终剩余票数10000，我们可以看出结果与正确结果相同。<br><br>
3）一个生产者一个消费者线程同步。设置一个线程共享的缓冲区， char buf[10]。一个线程不断从键盘输入字符到buf,一个线程不断的把buf的内容输出到显示器。要求输出的和输入的字符和顺序完全一致。（在输出线程中，每次输出睡眠一秒钟，然后以不同的速度输入测试输出是否正确）。要求多次测试添加同步机制前后的实验效果。<br>
这是一个生产者消费者模型，信号量s1、s2, 分别用来表示buf缓存区中空闲单元的个数和buf缓存区中数据的个数，当缓冲区为空，输出阻塞，当没有空间时，输入阻塞。<br>
* 参考代码：<br>
```
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
	j = (j+1)%10;
	}
        return NULL;
}
```
执行结果:<br>
![image](https://github.com/16281022/-_16281022/blob/master/os3_5.png)<br><br>
4）（下面两个题目，选做一个）<br>
（1）在Pinto操作系统中，增加一个系统调用，系统调用名为test_system_call()。无输入参数，输出为在显示器中打印输出：Hello. This is my test system call. <br>
（2）进程通信问题。阅读并运行共享内存、管道、消息队列三种机制的代码<br>
实验测试<br>a）通过实验测试，验证共享内存的代码中，receiver能否正确读出sender发送的字符串？如果把其中互斥的代码删除，观察实验结果有何不同？如果在发送和接收进程中打印输出共享内存地址，他们是否相同，为什么？<br>b)有名管道和无名管道通信系统调用是否已经实现了同步机制？通过实验验证，发送者和接收者如何同步的。比如，在什么情况下，发送者会阻塞，什么情况下，接收者会阻塞？<br>c）消息通信系统调用是否已经实现了同步机制？通过实验验证，发送者和接收者如何同步的。比如，在什么情况下，发送者会阻塞，什么情况下，接收者会阻塞？<br>
