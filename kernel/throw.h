#ifndef __KERNEL_ERROR_HANDLING_
#define __KERNEL_ERROR_HANDLING_

#define KERNEL_HALT_SEGMENTATION        800004
#define KERNEL_HALT_OUT_OF_MEMORY       100008
#define KERNEL_HALT_BAD_ALLOCATION      500007
#define KERNEL_HALT_ACCESS_VIOLATION    800064


void kThrow(long int errorCode, uint32_t hardwareAddress);


#endif
