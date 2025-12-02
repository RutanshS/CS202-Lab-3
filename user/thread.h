#ifndef _THREAD_H_
#define _THREAD_H_

struct lock_t{
    uint locked;
};

void lock_init(struct lock_t* lock);
void lock_acquire(struct lock_t* lock);
void lock_release(struct lock_t* lock);

int thread_create(void *(start_routine)(void*), void *arg);

#endif