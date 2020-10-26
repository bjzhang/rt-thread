#include <stdatomic.h>
#include <stdint.h>
#include <mthread.h>
#include <assert.h>
#include <sys/syscall.h>

int mthread_spin_init(mthread_spinlock_t *lock)
{
    // TODO:
}
int mthread_spin_destroy(mthread_spinlock_t *lock) {
    // TODO:
}
int mthread_spin_trylock(mthread_spinlock_t *lock)
{
    // TODO:
}
int mthread_spin_lock(mthread_spinlock_t *lock)
{
    // TODO:
}
int mthread_spin_unlock(mthread_spinlock_t *lock)
{
    // TODO:
}

int mthread_mutex_init(mthread_mutex_t *lock)
{
    // TODO:
    lock->data = EFREE;
    return 0;
}
int mthread_mutex_destroy(mthread_mutex_t *lock) {
    // TODO: maybe more to be done
    // do nothing!
    return 0;
}
int mthread_mutex_trylock(mthread_mutex_t *lock) {
    // TODO: maybe more to be done
    return 0;
}
int mthread_mutex_lock(mthread_mutex_t *lock) {
    // TODO:
    atomic_long a = EBUSY;
    int locked = 0;
    while (a) {
        a = atomic_exchange_d(&(lock->data),a);
        if (a == EBUSY && !locked) {
            sys_futex_wait(lock,lock->data);
            locked = 1;
        }
    }
    return 0;
}
#define NUM_MAX_TASK 16
int mthread_mutex_unlock(mthread_mutex_t *lock)
{
    // TODO:
    atomic_long a = EFREE;
    atomic_exchange_d(&(lock->data),a);
    sys_futex_wakeup(lock,NUM_MAX_TASK); // MAX_TASK would be a safe value
    return 0;
}

int mthread_barrier_init(mthread_barrier_t * barrier, unsigned count)
{
    // TODO:
    barrier->go_lim = count;
    barrier->count = 0;
}
int mthread_barrier_wait(mthread_barrier_t *barrier)
{
    // TODO:
    atomic_long a = barrier->count;
    atomic_long ret;
    while ((ret = atomic_compare_exchange_d(&(barrier->count),a,a+1)) != a) {
        a = barrier->count;
    }
    if (ret+1 == barrier->go_lim) {
        barrier->count = 0;
        sys_futex_wakeup(&(barrier->count),NUM_MAX_TASK);
    } else {
        sys_futex_wait(&(barrier->count),barrier->go_lim-1);
    }
    
}
int mthread_barrier_destroy(mthread_barrier_t *barrier)
{
    // TODO:
}

void mthread_cond_init(mthread_cond_t *cond)
{
    // TODO:
}
void mthread_cond_destroy(mthread_cond_t *cond) {
    // TODO:
}
void mthread_cond_wait(mthread_cond_t *cond, mthread_mutex_t *mutex)
{
    // TODO:
    mthread_mutex_unlock(mutex);
    sys_futex_wait(&(cond->data), cond->data);
    mthread_mutex_lock(mutex);
}
void mthread_cond_signal(mthread_cond_t *cond)
{
    // TODO:
    sys_futex_wakeup(&(cond->data),1);
}
void mthread_cond_broadcast(mthread_cond_t *cond)
{
    // TODO:
    sys_futex_wakeup(&(cond->data),NUM_MAX_TASK);
}

void mthread_semaphore_init(mthread_semaphore_t *sem, int val)
{
    // TODO:
    sem->data = val;
    mthread_mutex_init(&(sem->lock)); // we are free to acquire this lock

}
void mthread_semaphore_up(mthread_semaphore_t *sem)
{
    // TODO:
    mthread_mutex_lock(&(sem->lock));
    int ready = sys_futex_wakeup(&(sem->data),NUM_MAX_TASK);
    if (ready == 0) ++(sem->data);
    mthread_mutex_unlock(&(sem->lock));
}
void mthread_semaphore_down(mthread_semaphore_t *sem)
{
    // TODO:
    mthread_mutex_lock(&(sem->lock));
    // we have acquired the binary lock
    // we are in critical session now
    if (sem->data > 0) {
        --(sem->data);
        // release binary lock
        mthread_mutex_unlock(&(sem->lock));
    } else {
        // release binary lock
        mthread_mutex_unlock(&(sem->lock));
        sys_futex_wait(&(sem->data),sem->data);

    }
    
}
