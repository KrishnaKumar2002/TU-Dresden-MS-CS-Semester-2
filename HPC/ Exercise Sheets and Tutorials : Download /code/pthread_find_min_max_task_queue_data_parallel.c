/**
 * Simple program for TU Dresden, CS, TI, HPC lecture
 * Copyright (C) 2018-2019 TU Dresden
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <pthread.h>
#include <sys/syscall.h>

#include "../../common/common.h"

#define MAX_TASKS 1024

struct compute_info_struct
{
    size_t min_index;
    size_t max_index;
    DATATYPE result;
    // padding to avoid cache coherence problems
    long something[32];
};

static DATATYPE* array;

/* tasks have different stages */
enum state_enum{
    TASK_JOINED, // the task was joined by someone after the data has been read and can be overwritten
    TASK_ADDED, // the task was added to the queue and waits to be scheduled
    TASK_RUNNING, // the task is currently scheduled
    TASK_FINISHED, // the task has finished processing
    TASK_CANCEL_THREAD // this is a special task and means that one worker thread should be closed
};
struct task
{
     void* (*function)(void* ); // a function for this task
     void*  argument; // the argument for the function
     volatile void* return_value; // the computed return value
     volatile enum state_enum state; // the current state of the task 
};

struct task_queue
{
    /* stores the tasks */
    struct task tasks[MAX_TASKS];
    /* at this position we add new tasks */
    volatile int head;
    /* from this position we read the next task */
    volatile int next_to_process;
    /* this is the position where the last task joined */
    volatile int tail_joined;
};

static struct task_queue queue;
static pthread_mutex_t queue_mutex =PTHREAD_MUTEX_INITIALIZER;
static int usleep_time = 1;

/* we add new tasks to head */
int task_queue_add_task(void* (*function)(void* ),void*  argument){
    /* busy wait until there is space in the array to write the new task to */
    int can_add;
    do {
        can_add = 1;
        while (queue.tail_joined == queue.head){ // without lock to improve performance
            usleep(usleep_time);
        }
        pthread_mutex_lock(&queue_mutex);
        if ( queue.tail_joined == queue.head) {
            can_add = 0;
            pthread_mutex_unlock(&queue_mutex);
        }
    } while (!can_add);

    /* now we add it to the queue */
    int task_number = queue.head;
    queue.tasks[queue.head].function=function;
    queue.tasks[queue.head].argument=argument;
    queue.tasks[queue.head].return_value=NULL;
    queue.tasks[queue.head].state=TASK_ADDED;
    queue.head=(queue.head+1)%MAX_TASKS;
    pthread_mutex_unlock(&queue_mutex);
    return task_number;
}

/* we get new tasks from tail by getting the index in the task_queue */
int task_queue_get_task(){
    /* busy wait until there is space in the array to write the new task to */
    int task_index;
    int new_task;
    do {
        new_task = 1;
        while (queue.next_to_process == queue.head){ // without lock to improve performance
            usleep(usleep_time);
        }
        pthread_mutex_lock(&queue_mutex);
        if ( queue.next_to_process == queue.head) {
            new_task = 0;
            pthread_mutex_unlock(&queue_mutex);
        }
    } while (!new_task);
    task_index = queue.next_to_process;
    if ( queue.tasks[task_index].state != TASK_CANCEL_THREAD)
        queue.tasks[task_index].state = TASK_RUNNING;
    queue.next_to_process=(queue.next_to_process+1)%MAX_TASKS;
    pthread_mutex_unlock(&queue_mutex);
    return task_index;
}

/* define a task as finished (to be called by worker thread) */
void task_queue_finish_task(void* return_value, int index){
    pthread_mutex_lock(&queue_mutex);
    /* first we set finished for this task as well as the returned value */
    queue.tasks[index].return_value=return_value;
    queue.tasks[index].state=TASK_FINISHED;
    pthread_mutex_unlock(&queue_mutex);
}

/* wait until some tasks are finished and mark them as joined */
void task_queue_wait_for_tasks_and_join(int* task_numbers, void** return_values, int task_numbers_len){
    for (int i=0;i<task_numbers_len;i++){
        switch(queue.tasks[task_numbers[i]].state){
            case TASK_ADDED: /* fall-through */
            case TASK_RUNNING:
                /* here we wait for the task to be finished */
                while (queue.tasks[task_numbers[i]].state != TASK_FINISHED){ // without lock to improve performance
                    usleep(usleep_time);
                }
                break;
            case TASK_FINISHED:
                /* here we set the reurn value and change the state */
                pthread_mutex_lock(&queue_mutex);
                if (return_values != NULL)
                    return_values[task_numbers[i]] =queue.tasks[task_numbers[i]].return_value;
                queue.tasks[task_numbers[i]].state = TASK_JOINED;
                /* forward the joined tail */
                if (task_numbers[i] == queue.tail_joined){
                    int last_joined = i;
                    for (last_joined=task_numbers[i];queue.tasks[task_numbers[last_joined]].state!=TASK_JOINED;last_joined++);
                    queue.tail_joined = last_joined;
                }
                pthread_mutex_unlock(&queue_mutex);
                break;
            case TASK_JOINED:
                /* seems like someone wants to join again :/ */
                pthread_mutex_lock(&queue_mutex);
                if (return_values != NULL)
                    return_values[task_numbers[i]] =queue.tasks[task_numbers[i]].return_value;
                /* gives us a chance to forward the joined tail */
                if (task_numbers[i] == queue.tail_joined){
                    int last_joined = i;
                    for (last_joined=task_numbers[i];queue.tasks[task_numbers[last_joined]].state!=TASK_JOINED;last_joined++);
                    queue.tail_joined = last_joined;
                }
                pthread_mutex_unlock(&queue_mutex);
                break;
            default:
                /* This should not happen */
                assert(NULL);
        }
    }
}

/* wait until some tasks are finished */
void task_queue_wait_for_tasks(int* task_numbers, void** return_values, int task_numbers_len){
    for (int i=0;i<task_numbers_len;i++){
        switch(queue.tasks[task_numbers[i]].state){
            case TASK_ADDED: /* fall-through */
            case TASK_RUNNING:
                /* here we wait for the task to be finished */
                while (queue.tasks[task_numbers[i]].state != TASK_FINISHED){ // without lock to improve performance
                    usleep(usleep_time);
                }
                break;
            case TASK_FINISHED:
                pthread_mutex_lock(&queue_mutex);
                if (return_values != NULL)
                    return_values[task_numbers[i]] =queue.tasks[task_numbers[i]].return_value;
                pthread_mutex_unlock(&queue_mutex);
                break;
            case TASK_JOINED:
                pthread_mutex_lock(&queue_mutex);
                if (return_values != NULL)
                    return_values[task_numbers[i]] =queue.tasks[task_numbers[i]].return_value;
                pthread_mutex_unlock(&queue_mutex);
                break;
            default:
                /* This should not happen */
                assert(NULL);
        }
    }
}

/* Close one of the threads that are working on the queue */
void task_queue_close_one_thread(){
    /* busy wait until there is space in the array to write the new task to */
    int can_add;
    do {
        can_add = 1;
        while (queue.tail_joined == queue.head); // without lock to improve performance
        pthread_mutex_lock(&queue_mutex);
        if ( queue.tail_joined == queue.head) {
            can_add = 0;
            pthread_mutex_unlock(&queue_mutex);
        }
    } while (!can_add);

    /* now we add it to the queue */
    queue.tasks[queue.head].return_value=NULL;
    queue.tasks[queue.head].state=TASK_CANCEL_THREAD;
    queue.head=(queue.head+1)%MAX_TASKS;
    pthread_mutex_unlock(&queue_mutex);
}
/* This is executed by worker threads which get tasks and process them */
void* thread_work(void* ignored){
    while(1){
        int task_number = task_queue_get_task();
        if (queue.tasks[task_number].state == TASK_CANCEL_THREAD ) return NULL;
        void* ret = queue.tasks[task_number].function(queue.tasks[task_number].argument);
        task_queue_finish_task(ret,task_number);
    }
    return NULL;
}

void* find_min(void* thread_info_vp)
{
    struct compute_info_struct * ti =
            (struct compute_info_struct *) thread_info_vp;
    // highest possible number is RAND_MAX/2, so assume a higher number
    ti->result = (DATATYPE) RAND_MAX;
    for (size_t i = ti->min_index; i < ti->max_index; i++)
    {
        if (array[i] < ti->result)
            ti->result = array[i];
    }
    return NULL;
}

void* find_max(void* thread_info_vp)
{
    struct compute_info_struct * ti =
            (struct compute_info_struct *) thread_info_vp;
    // lowest possible number is -RAND_MAX/2, so assume a lower number
    ti->result = (DATATYPE) -RAND_MAX;
    for (size_t i = ti->min_index; i < ti->max_index; i++)
    {
        if (array[i] > ti->result)
            ti->result = array[i];
    }
    return NULL;
}

void* get_sum(void* thread_info_vp)
{
    struct compute_info_struct * ti =
            (struct compute_info_struct *) thread_info_vp;
    ti->result = (DATATYPE) 0;
    for (size_t i = ti->min_index; i < ti->max_index; i++)
    {
        ti->result += array[i];
    }
    return NULL;
}

struct compute_info_struct my_info;

int main(int argc, char** argv)
{
    long nr_threads = 1;
    if (getenv("NR_THREADS") != NULL)
        nr_threads = atoi(getenv("NR_THREADS"));
    if (getenv("USLEEP_TIME") != NULL)
        usleep_time = atoi(getenv("USLEEP_TIME"));

    queue.tail_joined = MAX_TASKS-1;
    pthread_t child_threads[ nr_threads ];
    for (int t=0;t<nr_threads;t++){
        pthread_create(&child_threads[t],NULL,thread_work,NULL);
    }

    arguments_t* arguments = parse_arguments(argc, argv,
            "Find min/max/sum in a 1-d array\n");

    array = create_data(arguments->size_in_byte);
    fill_data(array,arguments->size_in_byte);

    /*
     * one info for each possible parallel task
     */
    struct compute_info_struct * my_info = malloc ( MAX_TASKS * sizeof ( struct compute_info_struct ) );

    /* we will only have nr_threads tasks in parallel, but ... */
    for (int t=0;t<MAX_TASKS;t++){
        my_info[t].min_index = t*(arguments->length_of_array)/nr_threads;
        my_info[t].max_index = ((t+1)*(arguments->length_of_array))/nr_threads;
        my_info[t].result = 0;
    }
    int current_tasks[nr_threads];


    uint64_t start_time = get_time_in_us();
    for (size_t i = 0; i < arguments->nr_repetitions; i++)
    {
        /* please note that the parent thread does not participate in computation */
        /* create one task for each thread */
        for (int t=0;t<nr_threads;t++){
            current_tasks[t] = task_queue_add_task(find_min,&my_info[t]);
        }
        /* wait for compute threads to finish tasks */
        for (int t=0;t<nr_threads;t++){
            task_queue_wait_for_tasks(current_tasks,NULL,nr_threads);
        }
        /* merge results */
        DATATYPE mymin=__DBL_NORM_MAX__;
        for (int t=0;t<nr_threads;t++)
            if (my_info[t].result < mymin)
                mymin = my_info[t].result;
        /* cleanup tasks from task queue */
        for (int t=0;t<nr_threads;t++){
            task_queue_wait_for_tasks_and_join(current_tasks,NULL,nr_threads);
        }
        /* please note that the parent thread does not participate in computation */
        for (int t=0;t<nr_threads;t++){
            current_tasks[t] = task_queue_add_task(find_max,&my_info[t]);
        }
        /* wait for compute threads to finish tasks */
        for (int t=0;t<nr_threads;t++){
            task_queue_wait_for_tasks(current_tasks,NULL,nr_threads);
        }
        /* merge results */
        DATATYPE mymax=-__DBL_NORM_MAX__;
        for (int t=0;t<nr_threads;t++)
            if (my_info[t].result > mymax)
                mymax = my_info[t].result;

        /* cleanup tasks from task queue */
        for (int t=0;t<nr_threads;t++){
            task_queue_wait_for_tasks_and_join(current_tasks,NULL,nr_threads);
        }
        /* please note that the parent thread does not participate in computation */
        for (int t=0;t<nr_threads;t++){
            current_tasks[t] = task_queue_add_task(get_sum,&my_info[t]);
        }
        /* wait for compute threads to finish tasks */
        for (int t=0;t<nr_threads;t++){
            task_queue_wait_for_tasks(current_tasks,NULL,nr_threads);
        }
        /* merge results */
        DATATYPE mysum=0;
        for (int t=0;t<nr_threads;t++)
            mysum += my_info[t].result;


        /* cleanup tasks from task queue */
        for (int t=0;t<nr_threads;t++){
            task_queue_wait_for_tasks_and_join(current_tasks,NULL,nr_threads);
        }
        if (arguments->verbose){
            fprintf(arguments->output, "Min=%e , ", (double) mymin);
            fprintf(arguments->output, "Max=%e , ", (double) mymax);
            fprintf(arguments->output, "Sum=%e\n", (double) mysum);
        }
    }

    uint64_t end_time = get_time_in_us();

    printf("Getting min/max/sum of a %zu-bytes-sized 1-d array (" TOSTRING(DATATYPE) ") took %f microseconds\n", arguments->size_in_byte, (double)(end_time-start_time) / (double) arguments->nr_repetitions );

    for (int t=0;t<nr_threads;t++){
        task_queue_close_one_thread();
    }
    for (int t=0;t<nr_threads;t++){
        pthread_join(child_threads[t],NULL);
    }
    free_data(array);
    free(my_info);
}
