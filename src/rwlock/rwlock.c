#include <pthread.h>

#include "rwlock.h"

void rdlock(my_rwlock_t *p) {
    pthread_mutex_lock(&p->mutex);
    p->rlock_wait_c++;
    while (p->w_locked != 0) {
        pthread_cond_wait(&p->w_cv, &p->mutex);
    }
    --p->rlock_wait_c;
    ++p->r_locked_c;
    pthread_mutex_unlock(&p->mutex);
}

void wrlock(my_rwlock_t *p) {
    pthread_mutex_lock(&p->mutex);
    ++p->wlock_wait_c;
    if (p->w_locked) {
        while (p->w_locked != 0) {
            pthread_cond_wait(&p->w_cv, &p->mutex);
        }
    } else {
        while (p->r_locked_c > 0) {
            pthread_cond_wait(&p->r_cv, &p->mutex);
        }
    }
    --p->wlock_wait_c;
    p->w_locked = 1;
    pthread_mutex_unlock(&p->mutex);
}

void unlock(my_rwlock_t *p) {
    pthread_mutex_lock(&p->mutex);
    if (p->w_locked) {
        p->w_locked = 0;
        if (p->wlock_wait_c > 0) {
            pthread_cond_signal(&p->w_cv);
        } else if (p->rlock_wait_c > 0) {
            pthread_cond_broadcast(&p->w_cv);
        }
    } else if (p->r_locked_c) {
        --p->r_locked_c;
        if (!p->r_locked_c) {
            pthread_cond_signal(&p->r_cv);
        }
    }
    pthread_mutex_unlock(&p->mutex);
}
