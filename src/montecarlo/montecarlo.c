//
// Created by pravi on 21.10.2024.
//
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <string.h>
#include <sys/types.h>

#include "montecarlo.h"
#include "timer.h"

#define MC_NUM_ARGS 3

static double_t get_random_point() {
    return (double_t) rand() / (double_t) RAND_MAX; // NOLINT(cert-msc30-c, cert-msc50-cpp)
}

static void *routine(const void *vargs) {
    // Monte-carlo method
    uint64_t inside_circle_count = 0;
    uint64_t *res = malloc(sizeof(uint64_t));

    const pthread_args_t *args = vargs;
    const uint64_t arr_size = args->arr_size;

    for (uint64_t i = 0; i < arr_size; ++i) {
        const point_t p = args->points_arr[i];
        const double_t dist = p.x * p.x + p.y * p.y;
        if (dist <= 1.0) {
            ++inside_circle_count;
        }
    }
    *res = inside_circle_count;

    return res;
}

int montecarlo(const int argc, char *argv[]) {
    static point_t *points_arr;
    uint64_t inside_circle_count = 0;
    double start_time, end_time;

    if (argc != MC_NUM_ARGS) {
        fprintf(stderr, "Usage:\n%s [nthreads] [ntrials]\n", argv[0]);
        return -1;
    }

    srand(time(nullptr)); // NOLINT(cert-msc32-c, cert-msc51-cpp)

    uint64_t thread_count = strtoll(argv[1], nullptr, 10);
    const uint64_t throw_count = strtoll(argv[2], nullptr, 10);

    thread_count = thread_count >= throw_count >> 1 ? 1 : thread_count;

    pthread_t *thread_handler = malloc(thread_count * sizeof(pthread_t));

    points_arr = malloc(sizeof(point_t) * throw_count);
    memset(points_arr, 0, sizeof(point_t) * throw_count);

    for (uint64_t i = 0; i < throw_count; ++i) {
        const point_t p = {
            .x = get_random_point(),
            .y = get_random_point()
        };
        points_arr[i] = p;
    }

    const uint64_t single_thread_arr_size = throw_count / thread_count;
    const uint64_t last_thread_arr_size = single_thread_arr_size + (thread_count == 1 ? 0 : throw_count % thread_count);

    pthread_args_t *thread_args_arr = malloc(thread_count * sizeof(pthread_args_t));
    uint64_t **result = malloc(thread_count * sizeof(uint64_t *));

    for (uint64_t i = 0; i < thread_count; ++i) {
        pthread_args_t *args = thread_args_arr + i;
        args->tid = i;
        args->arr_size = i == thread_count - 1 ? last_thread_arr_size : single_thread_arr_size;
        args->points_arr = points_arr + i * single_thread_arr_size;
    }

    GET_TIME(start_time);
    for (uint64_t i = 0; i < thread_count; ++i) {
        const uint8_t err = pthread_create(thread_handler + i, nullptr, routine, &thread_args_arr[i]);
        if (err) {
            fprintf(stderr, "Creating %llu pthread failed\n", i);
        }
    }

    for (uint64_t i = 0; i < thread_count; ++i) {
        pthread_join(thread_handler[i], (void **) &result[i]);
    }
    GET_TIME(end_time);

    for (uint64_t i = 0; i < thread_count; ++i) {
        inside_circle_count += *result[i];
        free(result[i]);
    }

    printf("Estimated PI = %lf\n", 4.0 * ((double_t) inside_circle_count / throw_count)); // NOLINT
    printf("Done in %lfs ( %llu threads, %llu trials )\n", end_time - start_time, thread_count, throw_count);

    free(thread_handler);
    free(points_arr);
    free(thread_args_arr);

    return 0;
}
