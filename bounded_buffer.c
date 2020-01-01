#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
long compare_and_set(long *flag, long expected, long new);
    asm( /* compile with 64-bit compiler*/
    "compare_and_set:;"
    "movq %rsi,%rax;"
    "lock cmpxchgq %rdx,(%rdi);"
    "retq;"
);
long flag=0,expected=0,new=1;
void waite(int* mutex)
{
    while(compare_and_set(&flag, expected, new)!=0);
    *mutex--;
    flag = 0;
}
void signal(int* mutex)
{
    while(compare_and_set(&flag, expected,new)!=0);
    *mutex ++;
    flag =0;
}

#define BUFFER_SIZE 10 // defining buffer size
//  shared variables 
int buffer[BUFFER_SIZE] ={0};
int full=0,empty=BUFFER_SIZE,mutex=1;
int counter=0,value=0 ;
int in=0,out=0;
void readdata(int val);

// functions prototypes 
void *Producer(void*);
void * Consumer(void *);

int main()
{
    pthread_t t1,t2;
   for(int i=0;i<=BUFFER_SIZE;i++)
   {
       pthread_create(&t1, NULL,&Producer,NULL);
   }
   for(int i =0; i<=BUFFER_SIZE; i++)
   {
       pthread_create(&t2,NULL,&Consumer,NULL);
   }
   pthread_join(t1,NULL);
   pthread_join(t2,NULL);

}
// producer function will produce a value in the buffer 
void *Producer(void* ptr)
{
    int i=0;
    do
    {
        waite(&empty);  // wait until empty > 0 and then decrement 'empty
        waite(&mutex); // lock
        if(counter == BUFFER_SIZE)
        {
            //  do nothing 
            sleep(1);
        }
        else
        {   
            // critical section
         buffer[in] = i;
         printf("Producer Produce := %d \n", buffer[in]);
         in = (in+1)%BUFFER_SIZE;
        }
        counter++;
        signal(&mutex); //  relase lock
        signal(&full);  // increment 'full'
        i++;
    } while (1);
    
}

// consumer will read the value from buffer what value has been produce by producer
void * Consumer(void *ptr)
{
    do
    {
       
        waite(&full); // wait until full > 0 and then decrement 'full'
        waite(&mutex); //lock
        if(counter <=0)
        {
            // do nothing
            sleep(1);
        }else
        {
            // critical section
            readdata(buffer[out]);
            out =  (out+1)%BUFFER_SIZE;
        }
        counter--;
        signal(&mutex); // relase lock
        signal(&empty); // increment 'empty'
    } while (1);
    
}
void readdata(int val)
{
    printf("Consumer Consume := %d \n",  val);
}