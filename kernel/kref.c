<<<<<<< HEAD
#include <avr/io.h>
#include <stddef.h>

#include <kernel/kernel.h>

void kRefInit(kRef* rc) {
    // Initial count of 1
    rc->count = 1;
}

void kRefInc(kRef* rc) {
    if (rc != NULL) {
        rc->count++;
    }
}

uint8_t kRefDec(kRef* rc) {
    if (rc != NULL && rc->count > 0) {
        rc->count--;
    }
    return rc->count;
}

uint8_t kRefCheck(kRef* rc) {
    return (rc != NULL && rc->count == 0);
}

=======
#include <avr/io.h>
#include <stddef.h>

#include <kernel/kernel.h>

void kRefInit(kRef* rc) {
    // Initial count of 1
    rc->count = 1;
}

void kRefInc(kRef* rc) {
    if (rc != NULL) {
        rc->count++;
    }
}

uint8_t kRefDec(kRef* rc) {
    if (rc != NULL && rc->count > 0) {
        rc->count--;
    }
    return rc->count;
}

uint8_t kRefCheck(kRef* rc) {
    return (rc != NULL && rc->count == 0);
}

>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
