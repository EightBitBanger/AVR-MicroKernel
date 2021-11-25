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

