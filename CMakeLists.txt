cmake_minimum_required(VERSION 3.29)
project(hse_dc_lab1 C)

set(CMAKE_C_STANDARD 23)

add_executable(mandelbrot
        src/mandelbrot/main.c
        src/mandelbrot/mandelbrot.c
        src/mandelbrot/mandelbrot.h
        src/mandelbrot/timer.h
)

add_executable(montecarlo
        src/montecarlo/main.c
        src/montecarlo/montecarlo.c
        src/montecarlo/montecarlo.h
        src/montecarlo/timer.h
)

add_executable(rwlock
        src/rwlock/my_rand.c
        src/rwlock/my_rand.h
        src/rwlock/pth_ll_rwl.c
        src/rwlock/rwlock.c
        src/rwlock/rwlock.h
        src/rwlock/timer.h
)

add_executable(my_rwlock
        src/rwlock/my_rand.c
        src/rwlock/my_rand.h
        src/rwlock/pth_ll_rwl.c
        src/rwlock/rwlock.c
        src/rwlock/rwlock.h
        src/rwlock/timer.h
)
target_compile_definitions(my_rwlock PRIVATE MY_RWLOCK)
