/**
* Load balancer that gets set amount of jobs and then spins up an instance host to process the jobs
*
* Completion time: 20 Hours
*
* @author Greg Mooney
* @version 1.0
*/

#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "LoadBalancer.h"
#include "InstanceHost.h"

typedef struct job_node job_node;

struct balancer {
    int batch_size;
    int num_jobs;
    job_node* list;
    host* host;
    pthread_mutex_t lock;
};

/**
 * Initializes the load balancer. Takes batch size as parameter.
 */
balancer* balancer_create(int batch_size) {
    balancer* new_balancer = malloc(sizeof(balancer));
    new_balancer->list = malloc(sizeof(job_node) * batch_size);
    new_balancer->host = host_create();
    new_balancer->batch_size = batch_size;
    pthread_mutex_init(&new_balancer->lock, NULL);

    return new_balancer;
}

/**
 * Shuts down the load balancer. Ensures any outstanding batches have
 * completed.
 */
void balancer_destroy(balancer** lb) {
    pthread_mutex_lock(&(*lb)->lock);
    if ((*lb)->num_jobs > 0) {
        host_request_instance((*lb)->host, (*lb)->list);
        (*lb)->num_jobs = 0;

    }
    pthread_mutex_unlock(&(*lb)->lock);

    pthread_mutex_destroy(&(*lb)->lock);

    job_node* tmp = (*lb)->list;
    host_destroy(&(*lb)->host);

    while (tmp != NULL) {
        job_node* node = tmp->next;
        free(tmp);
        tmp = node;
    }



    free(*lb);
    *lb = NULL;


}

/**
 * Adds a job to the load balancer. If enough jobs have been added to fill a
 * batch, will request a new instance from InstanceHost. When job is complete,
 * *data_return will be updated with the result.
 *
 * @param user_id the id of the user making the request.
 * @param data the data the user wants to process.
 * @param data_return a pointer to a location to store the result of processing.
 */
void balancer_add_job(balancer* lb, int user_id, int data, int* data_return) {
    pthread_mutex_lock(&lb->lock);
    printf("LoadBalancer: Recieved new job from user #%d to process data = %d and "
           "store it at %p.\n", user_id, data, data_return);

    job_node* new_node = malloc(sizeof(job_node));
    new_node->user_id = user_id;
    new_node->data = data;
    new_node->data_result = data_return;
    new_node->next = NULL;

    // first job of batch
    if (lb->num_jobs == 0) {
        lb->list = new_node;
    }
    // putting new job on top of list
    else {
        new_node->next = lb->list;
        lb->list = new_node;
    }
    // increment number of jobs after adding one
    lb->num_jobs++;

    // if batch is full send it to host
    if (lb->num_jobs == lb->batch_size) {
        host_request_instance(lb->host, lb->list);
        // reset count
        lb->num_jobs = 0;
    }
    pthread_mutex_unlock(&lb->lock);
}

void print_lists(balancer *lb) {
    job_node * tmp;
    tmp = lb->list;
    do {
        printf("List: %d\n", tmp->data);
        tmp = tmp->next;
    } while (tmp != NULL);
}