#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"
#include "user/thread.h"

void lock_init(struct lock_t* lock){
    lock->locked = 0;
}

void lock_acquire(struct lock_t* lock){
    while(__sync_lock_test_and_set(&lock->locked, 1) != 0){
        ;
    }
}

void lock_release(struct lock_t* lock){
    __sync_lock_release(&lock->locked);
}

int 
thread_create(void *(start_routine)(void*), void *arg)
{
    void* stack = malloc(PGSIZE);
    if(stack==0){
        return -1;
    }

    void* stack_top = stack + PGSIZE;
    int pid = clone(stack_top);

    if(pid < 0){
        free(stack);
        return -1;
    }

    if(pid == 0){
        start_routine(arg);
        exit(0);
    }else{
        return 0;
    }
}
