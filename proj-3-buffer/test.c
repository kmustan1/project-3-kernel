#include "buffer.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/kernel.h>
#include <sys/syscall.h> 
#include <linux/delay.h>   //Used for creating delays between producer/consumer
#include <pthread.h>

#define __NR_init_buffer_421 442
#define __NR_enqueue_buffer_421 443
#define __NR_dequeue_buffer_421 444
#define __NR_delete_buffer_421 445

long init_buffer_421_syscall(void)
{
	return syscall(__NR_init_buffer_421);
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

int main(int argc, char *argv[])
{

}
