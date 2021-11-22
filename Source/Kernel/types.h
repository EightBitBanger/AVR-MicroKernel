//
// Define types

// Nullification
void    nullfunction(void) {return;};
uint8_t NULLvariable = 0;
#define nullptr 0x00000

#define NULL    NULLvariable
#define null    NULLvariable

// Address pointer
union Pointer {
	
#ifdef _32_BIT_POINTERS__
	char byte[4];
	uint16_t word[2];
	uint32_t dword;
	uint32_t address;
#endif
#ifdef _64_BIT_POINTERS__
	char byte[8];
	uint16_t word[4];
	uint32_t dword[2];
	uint64_t address;
#endif
	
	Pointer() {address=0;}
	
#ifdef _32_BIT_POINTERS__
	Pointer(uint32_t newAddress) {address=newAddress;}
#endif
#ifdef _64_BIT_POINTERS__
	Pointer(uint64_t newAddress) {address=newAddress;}
#endif
	
};


