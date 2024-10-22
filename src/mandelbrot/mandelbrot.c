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
#include <complex.h>

#include "mandelbrot.h"
#include "timer.h"

#define MB_NUM_ARGS 3
#define MB_ITERATIONS 4000
#define MB_OUTPUT_FILE "./mandelbrot_output.csv"

static uint64_t g_npoints;
static uint64_t g_mandelbrot_npoints = 0;
static complex double *g_mandelbrot_points_arr;
pthread_mutex_t g_mutex;

static int in_set(const double complex c) {
    double complex z = 0;

    for (uint16_t i = 0; i < MB_ITERATIONS; i++) {
        z = z * z + c;
        if (cabs(z) >= 2.0) {
            return 0;
        }
    }
    return 1;
}

static void routine(void *vargs) {
    // Mandelbrot set
    double complex c = 0;
    const auto args = (pthread_args_t *) vargs;
    const double_t end = args->x_end;

    for (double_t x = args->x_start; x < end; x += 0.00015) {
        for (double_t y = -1; y < 1; y += 0.00015) {
            c = x + y * I;
            if (in_set(c)) {
                pthread_mutex_lock(&g_mutex);
                if (g_mandelbrot_npoints >= g_npoints) {
                    pthread_mutex_unlock(&g_mutex);
                    break;
                }

                g_mandelbrot_points_arr[g_mandelbrot_npoints] = c;
                g_mandelbrot_npoints++;

                pthread_mutex_unlock(&g_mutex);
            }
        }
        if (g_mandelbrot_npoints >= g_npoints) {
            break;
        }
    }
    pthread_exit(NULL);
}


int mandelbrot(const int argc, char *argv[]) {
    double start, finish;

    if (argc != MB_NUM_ARGS) {
        fprintf(stderr, "Usage:\n%s [nthreads] [ntrials]\n", argv[0]);
        return -1;
    }

    const uint64_t thread_count = strtoll(argv[1], nullptr, 10);
    g_npoints = strtoll(argv[2], nullptr, 10);

    const double_t x_part_size = 4.0 / (double_t) thread_count;

    pthread_t *thread_handler = malloc(thread_count * sizeof(pthread_t));
    g_mandelbrot_points_arr = malloc(sizeof(complex double) * g_npoints);
    pthread_mutex_init(&g_mutex, nullptr);
    pthread_args_t *thread_args_arr = malloc(thread_count * sizeof(pthread_args_t));

    for (uint64_t i = 0; i < thread_count; i++) {
        pthread_args_t *args = thread_args_arr + i;
        args->tid = i;
        args->x_start = -2.0 + x_part_size * i;
        args->x_end = args->x_start + x_part_size;
    }

    GET_TIME(start);
    for (uint64_t i = 0; i < thread_count; i++) {
        const uint8_t err = pthread_create(thread_handler + i, nullptr, routine, &thread_args_arr[i]);
        if (err) {
            fprintf(stderr, "Creating %llu pthread failed\n", i);
        }
    }

    for (uint64_t i = 0; i < thread_count; i++) {
        pthread_join(thread_handler[i], nullptr);
    }
    GET_TIME(finish);

    FILE *file = fopen(MB_OUTPUT_FILE, "w");
    if (file != NULL) {
        for (uint64_t i = 0; i < g_mandelbrot_npoints; i++) {
            fprintf(file, "(%lf, %lf)\n", creal(g_mandelbrot_points_arr[i]), cimag(g_mandelbrot_points_arr[i]));
        }
    } else {
        fprintf(stderr, "Cannot open file %s\n", MB_OUTPUT_FILE);
    }
    fclose(file);

    printf("Check result in the %s\n", MB_OUTPUT_FILE);
    printf("Done in %lfs ( %llu threads, %llu points )\n", finish - start, thread_count, g_npoints);

    pthread_mutex_destroy(&g_mutex);
    free(thread_args_arr);
    free(g_mandelbrot_points_arr);
    free(thread_handler);

    return 0;
}
