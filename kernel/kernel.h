#ifndef __KERNEL_CORE_
#define __KERNEL_CORE_

#define PERIPHERAL_ADDRESS_BEGIN  0x40000

#define NUMBER_OF_PERIPHERALS  5

#include <kernel/bus.h>
#include <kernel/device.h>



void kernel_initiate(void);

struct Device* GetDevice(unsigned int deviceIndex);



int is_number(char* charPtr);
int is_letter(char* charPtr);

int is_uppercase(char* charPtr);
int is_lowercase(char* charPtr);

void make_uppercase(char* charPtr);
void make_lowercase(char* charPtr);

#endif
