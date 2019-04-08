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
![input_txt](https://github.com/16281022/os-lab3/blob/master/3.1.png)<br><br>
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
![image](https://github.com/16281022/os-lab3/blob/master/3.2.png)<br><br>
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
![image](https://github.com/16281022/os-lab3/blob/master/3.3.png)<br><br>
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
![image](https://github.com/16281022/os-lab3/blob/master/3.4.png)<br><br>
加入sleep：
![image](https://github.com/16281022/os-lab3/blob/master/3.5.png)<br><br>
4）（下面两个题目，选做一个）<br>
进程通信问题。阅读并运行共享内存、管道、消息队列三种机制的代码<br>
实验测试<br>a）通过实验测试，验证共享内存的代码中，receiver能否正确读出sender发送的字符串？如果把其中互斥的代码删除，观察实验结果有何不同？如果在发送和接收进程中打印输出共享内存地址，他们是否相同，为什么？<br>b)有名管道和无名管道通信系统调用是否已经实现了同步机制？通过实验验证，发送者和接收者如何同步的。比如，在什么情况下，发送者会阻塞，什么情况下，接收者会阻塞？<br>c）消息通信系统调用是否已经实现了同步机制？通过实验验证，发送者和接收者如何同步的。比如，在什么情况下，发送者会阻塞，什么情况下，接收者会阻塞？<br>
* （a）<br>
打印共享内存地址：<br>
```
printf("the sharing address is %p\n",shm_ptr);
```
测试结果，能正确读出sender发送的字符：<br>
![image](https://github.com/16281022/os-lab3/blob/master/3.6.png)<br><br>
将互斥代码删除(注释代码)，不会收到信息:<br>
![image](https://github.com/16281022/os-lab3/blob/master/3.7.png)<br><br>
* （b）<br>
有名管道和无名管道通信系统调用实现了同步机制。
无名管道：无名管道是一类特殊的文件，在内核中对应着一段特殊的内存空间，内核在这段内存空间中以循环队列的方式存储数据；无名管道的内核资源在通信双方退出后自动消失，无需人为回收；无名管道主要用于连通亲缘进程(父子进程)，用于双方的快捷通信，通信为单向通信<br>
![image](https://github.com/16281022/os-lab3/blob/master/3.8.png)<br><br>
有名管道：又称FIFO(Fisrt In First out), 是磁盘上的一个特殊文件，没有在磁盘存储真正的信息，其所存储的信息在内存中；通信双方进程结束后，自动丢弃FIFO中的信息，但其在磁盘的文件路径本身依然存在<br>
![image](https://github.com/16281022/os-lab3/blob/master/3.9.png)<br><br>
* 如果发送端发送信息后，接收端没有收到，则发送端阻塞。
* 如果发送端没有发送消息，则接收端阻塞
* 如果管道写满，则发送端阻塞。
* 如果管道为空，接收端阻塞
（c）<br>
消息队列是可用于同主机任意多进程的通信，但其可存放的数据有限，应用于少量的数据传递;他可以认为是一个消息链表。<br>
![image](https://github.com/16281022/os-lab3/blob/master/3.10.png)<br><br>
* 造成msgrcv()阻塞的情况:<br>
没有消息被写入队列时，接收方发生阻塞，等待消息发送方发送消息，直到超出规定时间结束。
* 造成msgsnd()等待的条件:<br>
当前消息大小与当前消息队列中的字节数之和超过了消息队列的总容量；当前消息队列的消息数大于等于消息队列的总容量。<br>
5）阅读Pintos操作系统，找到并阅读进程上下文切换的代码，说明实现的保存和恢复的上下文内容以及进程切换的工作流程。<br>
上下文切换（Context Switch） 或者环境切换在操作系统中，CPU切换到另一个进程需要保存当前进程的状态并恢复另一个进程的状态：当前运行任务转为就绪（或者挂起、删除）状态，另一个被选定的就绪任务成为当前任务。上下文切换包括保存当前任务的运行环境，恢复将要运行任务的运行环境。进程上下文用进程的PCB（进程控制块,也称为PCB，即任务控制块）表示，它包括进程状态，CPU寄存器的值等。通常通过执行一个状态保存来保存CPU当前状态，然后执行一个状态恢复重新开始运行。<br>
上下文切换的开销：
* 保存和恢复寄存器
* 切换地址空间（相关指令可能比较昂贵）
* 缓存和缓冲失效（高速缓存，缓冲区缓存，TLB）
schdule代码：<br>
```
  /* Schedules a new process.  At entry, interrupts must be off and
     the running process's state must have been changed from
     running to some other state.  This function finds another
     thread to run and switches to it.
  
     It's not safe to call printf() until thread_schedule_tail()
     has completed. */
  static void
  schedule (void)
 {
   struct thread *cur = running_thread ();
   struct thread *next = next_thread_to_run ();
   struct thread *prev = NULL;
 
   ASSERT (intr_get_level () == INTR_OFF);
   ASSERT (cur->status != THREAD_RUNNING);
   ASSERT (is_thread (next));
 
   if (cur != next)
     prev = switch_threads (cur, next);
   thread_schedule_tail (prev);
}
```
首先获取当前线程cur和调用next_thread_to_run获取下一个要run的线程：<br>
```
  /* Chooses and returns the next thread to be scheduled.  Should
     return a thread from the run queue, unless the run queue is
     empty.  (If the running thread can continue running, then it
     will be in the run queue.)  If the run queue is empty, return
     idle_thread. */
  static struct thread *
  next_thread_to_run (void)
  {
    if (list_empty (&ready_list))
     return idle_thread;
   else
     return list_entry (list_pop_front (&ready_list), struct thread, elem);
 }
```
如果就绪队列空闲直接返回一个空闲线程指针， 否则拿就绪队列第一个线程出来返回。然后3个断言确保不能被中断， 当前线程是RUNNING_THREAD等。如果当前线程和下一个要跑的线程不是同一个的话调用switch_threads返回给prev。<br>
```
1 /* Switches from CUR, which must be the running thread, to NEXT,
2    which must also be running switch_threads(), returning CUR in
3    NEXT's context. */
4 struct thread *switch_threads (struct thread *cur, struct thread *next);
```
注意， 这个函数实现是用汇编语言实现的在threads/switch.S里：<br>
```
 1 #### struct thread *switch_threads (struct thread *cur, struct thread *next);
 2 ####
 3 #### Switches from CUR, which must be the running thread, to NEXT,
 4 #### which must also be running switch_threads(), returning CUR in
 5 #### NEXT's context.
 6 ####
 7 #### This function works by assuming that the thread we're switching
 8 #### into is also running switch_threads().  Thus, all it has to do is
 9 #### preserve a few registers on the stack, then switch stacks and
10 #### restore the registers.  As part of switching stacks we record the
11 #### current stack pointer in CUR's thread structure.
12 
13 .globl switch_threads
14 .func switch_threads
15 switch_threads:
16     # Save caller's register state.
17     #
18     # Note that the SVR4 ABI allows us to destroy %eax, %ecx, %edx,
19     # but requires us to preserve %ebx, %ebp, %esi, %edi.  See
20     # [SysV-ABI-386] pages 3-11 and 3-12 for details.
21     #
22     # This stack frame must match the one set up by thread_create()
23     # in size.
24     pushl %ebx
25     pushl %ebp
26     pushl %esi
27     pushl %edi
28 
29     # Get offsetof (struct thread, stack).
30 .globl thread_stack_ofs
31     mov thread_stack_ofs, %edx
32 
33     # Save current stack pointer to old thread's stack, if any.
34     movl SWITCH_CUR(%esp), %eax
35     movl %esp, (%eax,%edx,1)
36 
37     # Restore stack pointer from new thread's stack.
38     movl SWITCH_NEXT(%esp), %ecx
39     movl (%ecx,%edx,1), %esp
40 
41     # Restore caller's register state.
42     popl %edi
43     popl %esi
44     popl %ebp
45     popl %ebx
46         ret
47 .endfunc
```
 分析一下这个汇编代码： 先4个寄存器压栈保存寄存器状态（保护作用）， 这4个寄存器是switch_threads_frame的成员：<br>
 ```
  /* switch_thread()'s stack frame. */
  struct switch_threads_frame 
    {
      uint32_t edi;               /*  0: Saved %edi. */
      uint32_t esi;               /*  4: Saved %esi. */
      uint32_t ebp;               /*  8: Saved %ebp. */
      uint32_t ebx;               /* 12: Saved %ebx. */
      void (*eip) (void);         /* 16: Return address. */
      struct thread *cur;         /* 20: switch_threads()'s CUR argument. */
     struct thread *next;        /* 24: switch_threads()'s NEXT argument. */
   };
```
然后全局变量thread_stack_ofs记录线程和棧之间的间隙， 线程切换有个保存现场的过程，来看34,35行， 先把当前的线程指针放到eax中， 并把线程指针保存在相对基地址偏移量为edx的地址中。38,39： 切换到下一个线程的线程棧指针， 保存在ecx中， 再把这个线程相对基地址偏移量edx地址（上一次保存现场的时候存放的）放到esp当中继续执行。这里ecx, eax起容器的作用， edx指向当前现场保存的地址偏移量。简单来说就是保存当前线程状态， 恢复新线程之前保存的线程状态。然后再把4个寄存器拿出来， 这个是硬件设计要求的， 必须保护switch_threads_frame里面的寄存器才可以destroy掉eax, edx, ecx。然后注意到现在eax(函数返回值是eax)就是被切换的线程棧指针。由此得到一个结论， schedule先把当前线程丢到就绪队列，然后把线程切换如果下一个线程和当前线程不一样的话。
