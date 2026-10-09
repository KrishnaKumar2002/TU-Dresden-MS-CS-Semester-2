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

#include <pthread.h>
#include <sys/syscall.h>

#include "../../common/common.h"

struct compute_info_struct
{
    size_t min_index;
    size_t max_index;
    DATATYPE result;
    // padding to avoid cache coherence problems
    long something[32];
};

static DATATYPE* array;

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
    // lowest possible number is -RAND_MAX/2, so assume a lower number
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
    arguments_t* arguments = parse_arguments(argc, argv,
            "Find min/max/sum in a 1-d array\n");

    array = create_data(arguments->size_in_byte);
    fill_data(array,arguments->size_in_byte);

    /*
     * Independent information for possible threads
     * default: 1 thread that looks at all elements --> malloc ( 1 * ...
     * TODO allocate for multiple threads
     */
    struct compute_info_struct * my_info = malloc ( 3 * sizeof ( struct compute_info_struct ) );

    /* we will have three tasks for three threads */
    for (int t=0;t<3;t++){
        my_info[t].min_index = 0;
        my_info[t].max_index = arguments->length_of_array;
        my_info[t].result = 0;
    }

    /* TODO assign work to other threads */
    /* ... */
    pthread_t child_threads[ 2 ];

    uint64_t start_time = get_time_in_us();
    for (size_t i = 0; i < arguments->nr_repetitions; i++)
    {
        /* child threads get min and max */
        pthread_create(&child_threads[0],NULL,find_min,&my_info[0]);
        pthread_create(&child_threads[1],NULL,find_max,&my_info[1]);

        /* parent thread gets sum */
        get_sum(&my_info[2]);

        /* join child threads */
        for (int t=0;t<2;t++)
            pthread_join(child_threads[t],NULL);


        if (arguments->verbose){
            fprintf(arguments->output, "Min=%e , ", (double) my_info[0].result);
            fprintf(arguments->output, "Max=%e , ", (double) my_info[1].result);
            fprintf(arguments->output, "Sum=%e\n", (double) my_info[2].result);
        }
    }

    uint64_t end_time = get_time_in_us();

    printf("Getting min/max/sum of a %zu-bytes-sized 1-d array (" TOSTRING(DATATYPE) ") took %f microseconds\n", arguments->size_in_byte, (double)(end_time-start_time) / (double) arguments->nr_repetitions );

    free_data(array);
    free(my_info);
}
