#include "buffer.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/kernel.h>
#include <sys/syscall.h> 
#include <pthread.h>
#include <sys/time.h>

#define __NR_init_buffer_421 442
#define __NR_enqueue_buffer_421 443
#define __NR_dequeue_buffer_421 444
#define __NR_delete_buffer_421 445

static sem_t mutex; 

long init_buffer_421_syscall(void)
{
	long result;
	result = syscall(__NR_init_buffer_421);
	
	if(result == 0) //Initalize mutex if return value of syscall is 0 (succeeded) 
	{
		sem_init(&mutex, 0, 1); 
		return result; 
	}
	
	return result; 
}

long enqueue_buffer_421_syscall(char* data)
{
	return syscall(__NR_enqueue_buffer_421, data); 
}

long dequeue_buffer_421_syscall(char* data)
{
	return syscall(__NR_dequeue_buffer_421, data);
}

long delete_buffer_421_syscall(void)
{
	return syscall(__NR_delete_buffer_421); 
}

void *producer(void *arg)
{
	arg = arg;//so that we don;t get the warning that we never use them
	
	char *data = (char*)malloc(sizeof(char)*DATA_LENGTH);
	int p = 0;
	
	//More random seeding method - source: guyrutenberg.com/2007/09/03/seeding-srand/ - Using current time can result in same seed if threads run fast enough that system 
	//time doesn't technically change
	struct timeval t1;
	gettimeofday(&t1, NULL); //Get current time of day
	srand(t1.tv_usec * t1.tv_sec); //Multiply current microseconds by seconds, use this value to seed 
	
	useconds_t sleepTime; //Will hold sleep time
	int base_time = (rand() % 11); //Base time, number between 0 and 10

	
	for (int i=0; i < 10000; i++)
	{
		int c = '0' + p;
		
		memset(data, c, DATA_LENGTH);
		data[1023] = '\0';//set the last index to null for printf later
		enqueue_buffer_421_syscall(data);
		p++;
		
		if (p>9) p = 0;
		
		base_time = (rand() % 11); //Base time, number between 0 and 10
	
		if(base_time == 10) //100,000 microseconds = 100 miliseconds
		{
			sleepTime = (useconds_t) base_time * 10000; 
		}
	
		else //Add a fractional value between 999 and 0 for values between 0 and 9999
		{
			useconds_t frac = rand() % ((9999 + 1 - 0) + 0);  
			sleepTime = (useconds_t) (base_time * 10000) + frac; 
		}
		
		usleep(sleepTime); //Sleep for any time between 0ms - 100ms
	}
	
	free(data);
	return NULL;
}

//consumer thread function
void *consumer(void *arg)
{
	arg = arg;
	char *data = (char*)malloc(sizeof(char)*DATA_LENGTH);
	
	//More random seeding method - source: guyrutenberg.com/2007/09/03/seeding-srand/ - Using current time can result in same seed if threads run fast enough that system 
	//time doesn't technically change
	struct timeval t1;
	gettimeofday(&t1, NULL); //Get current time of day
	srand(t1.tv_usec * t1.tv_sec); //Multiply current microseconds by seconds, use this value to seed 
	
	useconds_t sleepTime; //Will hold sleep time
	int base_time; 		  //Base time, number between 0 and 10
	
	for (int i =0; i < 10000; i++)
	{
		dequeue_buffer_421_syscall(data);
		
		base_time = (rand() % 11); //Base time, number between 0 and 10
	
		if(base_time == 10) //100,000 microseconds = 100 miliseconds
		{
			sleepTime = (useconds_t) base_time * 10000; 
		}
	
		else //Add a fractional value between 999 and 0 for values between 0 and 9999
		{
			useconds_t frac = rand() % ((9999 + 1 - 0) + 0);  
			sleepTime = (useconds_t) (base_time * 10000) + frac; 
		}
		
			usleep(sleepTime);
	}
	
	free(data);
	
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t producerID, consumerID; 
	void* thread_result;
	
	pthread_create(&producerID, NULL, producer, NULL);
	pthread_create(&consumerID, NULL, consumer, NULL);
	
	pthread_join(producerID, &thread_result);
	pthread_join(consumerID, &thread_result);
	
	delete_buffer_421_syscall();  
	
	printf("Finished\n"); 
	
	return 0;
}
