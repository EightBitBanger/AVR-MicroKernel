#ifndef __POINTER_WRAP_UP__
#define __POINTER_WRAP_UP__


#define nullptr 0x00000000

#ifndef _32_BIT_POINTERS__
 #ifndef _64_BIT_POINTERS__
  #define _32_BIT_POINTERS__
 #endif
#endif


union WrappedPointer {
	
#ifdef _32_BIT_POINTERS__
	char     byte  [4];
	uint8_t  byte_t[4];
	uint16_t word  [2];
	uint32_t dword;
	uint32_t address;
#endif
#ifdef _64_BIT_POINTERS__
	char     byte  [8];
	uint8_t  byte_t[8];
	uint16_t word  [4];
	uint32_t dword [2];
	uint64_t address;
#endif
	
	WrappedPointer() {address=0;}
	
#ifdef _32_BIT_POINTERS__
	WrappedPointer(uint32_t newAddress) {address=newAddress;}
#endif
#ifdef _64_BIT_POINTERS__
	WrappedPointer(uint64_t newAddress) {address=newAddress;}
#endif
	
};


#endif

