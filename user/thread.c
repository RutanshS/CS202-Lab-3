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

