<<<<<<< HEAD
#ifndef __KERNEL_MUTEX_LOCK_
#define __KERNEL_MUTEX_LOCK_

struct mutex {
    
    uint8_t lock;
    
};

uint8_t MutexLock(struct mutex* mux);
void MutexUnlock(struct mutex* mux);

#endif
=======
#ifndef __KERNEL_MUTEX_LOCK_
#define __KERNEL_MUTEX_LOCK_

struct mutex {
    
    uint8_t lock;
    
};

uint8_t MutexLock(struct mutex* mux);
void MutexUnlock(struct mutex* mux);

#endif
>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
