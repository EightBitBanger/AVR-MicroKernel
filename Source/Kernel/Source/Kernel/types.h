//
// Define types

// Nullification
void    nullfunction(void) {return;};
uint8_t nullchar = 0;
#define nullptr 0x00000

#define NULL    nullchar
#define NULL_f  nullfunction

// Driver function entry pointer
typedef void(*EntryPtr)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);


// Address pointer wrapper
union WrappedPointer {
	
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
	
	WrappedPointer() {address=0;}
	
#ifdef _32_BIT_POINTERS__
	WrappedPointer(uint32_t newAddress) {address=newAddress;}
#endif
#ifdef _64_BIT_POINTERS__
	WrappedPointer(uint64_t newAddress) {address=newAddress;}
#endif
	
};


