#ifndef __KERNEL_ERROR_HANDLING_
#define __KERNEL_ERROR_HANDLING_

#define HALT_SEGMENTATION_FAULT   800004
#define HALT_OUT_OF_MEMORY        100008
#define HALT_BAD_ALLOCATION       500007
#define HALT_ACCESS_VIOLATION     800064


void kThrow(long int errorCode, uint32_t hardwareAddress);


#endif
