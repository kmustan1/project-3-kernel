#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/semaphore.h> //Semaphores used for mutual exclusion
#include <linux/slab.h>    //Memory allocation in Kernel
#include <linux/kthread.h> //Used for threads in the kernel
#include <linux/delay.h>   //Used for creating delays between producer/consumer
#include <linux/random.h>  //Used for RNG for delays
#include "buffer.h"        //Buffer definitions 

static ring_buffer_421_t buffer;
static sem_t mutex;
static sem_t fill_count;
static sem_t empty_count;

SYSCALL_DEFINE0(init_buffer_421)
{
	// Note: You will need to initialize semaphores in this function.
    // Ensure we're not initializing a buffer that already exists.
    if (buffer.read || buffer.write) {
        printf("init_buffer_421(): Buffer already exists. Aborting.\n");
        return -1;
    }

    // Create the root node.
    node_421_t *node;
    node = (node_421_t *) malloc(sizeof(node_421_t));
    // Create the rest of the nodes, linking them all together.
    node_421_t *current;
    int i;
    current = node;
    // Note that we've already created one node, so i = 1.
    for (i = 1; i < SIZE_OF_BUFFER; i++) {
        current->next = (node_421_t *) malloc(sizeof(node_421_t));
        current = current->next;
    }
    // Complete the chain.
    current->next = node;
    buffer.read = node;
    buffer.write = node;
    buffer.length = 0;

    // Initialize your semaphores here.
    // TODO
    sem_init(&mutex, 0, 1);
    sem_init(&fill_count, 0, 0);//we should not dequeue if there in nothing filled
    sem_init(&empty_count, 0, 20);//start at 19 to make sure we have a negative value after 20 consecutive insertions

    return 0;
}

SYSCALL_DEFINE1(enqueue_buffer_421, char*, data){
	/ NOTE: You have to modify this function to use semaphores.
    if (!buffer.write) {
        printf("write_buffer_421(): The buffer does not exist. Aborting.\n");
        return -1;
    }

    //if(curr_value > 9) curr_value = 0; //Only insert ints 0-9

    //Item is already "produced" by loop, tell consumer to wait until producer is finished & decrement empty count
    sem_wait(&empty_count);
    sem_wait(&mutex);

    //Take produced item and put into array indices in node
    memcpy(buffer.write->data, data, DATA_LENGTH);
    buffer.write = buffer.write->next;
    buffer.length++;
    //Signal consumer that producer is finished, increment fill count and buffer length
    sem_post(&mutex);
    sem_post(&fill_count);
 

    return 0;
}

long dequeue_buffer_421(char * data) {
	// NOTE: Implement this function.
	return 0;
}

long delete_buffer_421(void) {
	// Tip: Don't call this while any process is waiting to enqueue or dequeue.
	if (!buffer.read) {
		printf("delete_buffer_421(): The buffer does not exist. Aborting.\n");
		return -1;
	}
	// Get rid of all existing nodes.
	node_421_t *temp;
	node_421_t *current = buffer.read->next;
	while (current != buffer.read) {
		temp = current->next;
		free(current);
		current = temp;
	}
	// Free the final node.
	free(current);
	current = NULL;
	// Reset the buffer.
	buffer.read = NULL;
	buffer.write = NULL;
	buffer.length = 0;
	return 0;
}

void print_semaphores(void) {
	// You can call this method to check the status of the semaphores.
	// Don't forget to initialize them first!
	// YOU DO NOT NEED TO IMPLEMENT THIS FOR KERNEL SPACE.
	int value;
	sem_getvalue(&mutex, &value);
	printf("sem_t mutex = %d\n", value);
	sem_getvalue(&fill_count, &value);
	printf("sem_t fill_count = %d\n", value);
	sem_getvalue(&empty_count, &value);
	printf("sem_t empty_count = %d\n", value);
	return;
}
