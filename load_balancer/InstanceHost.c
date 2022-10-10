/**
* Instance host that recieves batch of jobs from load balancer, process them, and gives the
* User the result
*
* Completion time: 20 Hours
*
* @author Greg Mooney
* @version 1.0
*/

#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "InstanceHost.h"


// Forward declarations
void* start_instance(void* param);

struct host {
struct job_node** batch_list;
pthread_mutex_t host_lock;
int num_batches;
};

typedef struct host_batch {
    struct job_node* batch;
    struct host* host;
} host_batch;

/**
* Initializes the host environment.
*/
host* host_create() {
    struct host* new_host = malloc(sizeof(host));
    pthread_mutex_init(&new_host->host_lock, NULL);
    new_host->num_batches = 0;
    return new_host;
}

/**
* Shuts down the host environment. Ensures any outstanding batches have
* completed.
*/
void host_destroy(host** h) {
    while ((*h)->num_batches > 0); // wait for all batches to end
    pthread_mutex_destroy(&(*h)->host_lock);
    free(*h);
    *h = NULL;
}

/**
* Creates a new server instance (i.e., thread) to handle processing the items
* contained in a batch (i.e., a listed list of job_node). InstanceHost will
* maintain a list of active instances, and if the host is requested to
* shutdown, ensures that all jobs are completed.
*
* @param job_batch_list A list containing the jobs in a batch to process.
*/
void host_request_instance(host* h, struct job_node* batch) {
    printf("LoadBalancer: Received batch and spinning up new instance.\n");
    pthread_t new_thread;

    host_batch* hb = malloc(sizeof(host_batch));
    hb->batch = batch;
    hb->host = h;

    h->num_batches++;
    pthread_create(&new_thread, NULL, start_instance, hb);

    pthread_join(new_thread, NULL);

}

void* start_instance(void* param) {
    host_batch* input = (host_batch *) param;
    struct job_node* in = input->batch;
    pthread_mutex_lock(&input->host->host_lock);

    while (in != NULL) {
        *in->data_result = in->data * in->data;
        in = in->next;

    }
    input->host->num_batches--;
    pthread_mutex_unlock(&input->host->host_lock);
    pthread_exit(0);
}
