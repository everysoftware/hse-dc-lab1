//
// Created by pravi on 21.10.2024.
//

#ifndef PI_CALC_H
#define PI_CALC_H

#include <stdint.h>
#include <math.h>


typedef struct {
    double_t x;
    double_t y;
} point_t;

typedef struct {
    uint64_t tid;

    point_t *points_arr;
    uint64_t arr_size;
} pthread_args_t;


int montecarlo(int argc, char *argv[]);
#endif //PI_CALC_H
