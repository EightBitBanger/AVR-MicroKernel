//
// Extended memory cache

#define _CACHE_SIZE__  8

struct CacheBuffer {
	
	uint32_t current_address;
	char cache[_CACHE_SIZE__];
	
	CacheBuffer() {current_address = _HEAP_END__;}
	
	char& operator[] (uint32_t address_pointer) {
		
		// Check out of range
		if ((address_pointer < current_address) || (address_pointer > (current_address + _CACHE_SIZE__))) {
			// Dump the cache back to memory
			for (uint8_t i=0; i<_CACHE_SIZE__; i++) {
				memory_write(current_address + i, cache[i]);
				memory_read(address_pointer + i, cache[i]);
			}
			current_address = address_pointer;
		}
		
		return cache[address_pointer - current_address];
	}
	
	// Dump the cache back to memory and load the new address
	void dump(uint32_t new_address) {
		
	}
	
};
CacheBuffer memory_cache;

