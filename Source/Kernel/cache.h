//
// Extended memory cache

#ifndef _CACHE_SIZE__
 #define _CACHE_SIZE__  32
#endif

struct CacheBuffer {
	
	uint32_t current_address;
	char cache[_CACHE_SIZE__];
	char protectionOverflowBuffer[1];
	
	CacheBuffer() {current_address = _STACK_END__;}
	
	char& operator[] (uint32_t new_pointer) {
		
		// Check segmentation fault
		if ((new_pointer < _STACK_BEGIN__) || (new_pointer >= _STACK_END__)) {
			memory_write(_KERNEL_FLAGS__, _KERNEL_STATE_SEG_FAULT__);
			return protectionOverflowBuffer[0];
		}
		
		// Check cache out of bounds
		if ((new_pointer < current_address) || (new_pointer >= (current_address + _CACHE_SIZE__))) {
			// Dump the cache back to memory
			for (uint8_t i=0; i<_CACHE_SIZE__; i++) {
				memory_write(current_address + i, cache[i]);
				memory_read(new_pointer + i, cache[i]);
			}
			current_address = new_pointer;
		}
		
		return cache[new_pointer - current_address];
	}
	
};
CacheBuffer memory_cache;

