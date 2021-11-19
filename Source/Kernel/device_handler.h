//
// Device handler

#define _DEVICE_INDEX__   (_KERNEL_BEGIN__ + 0x00000)

// Device address range
#define _DEVICE_ADDRESS_START__  0x40000
#define _DEVICE_ADDRESS_END__    0x80000

struct DeviceHandler {
	
	// Initiate the device handler
	void initiate(void) {
		
		memory_cache[0x00100] = 0x00;
		
		mem_zero(_DEVICE_INDEX__, 8);
		
		char identityByte=0xff;
		uint32_t address=_DEVICE_ADDRESS_START__;
		for (uint8_t i=0; i < 16; i++) {
			
			device_read(address, identityByte);
			
			if (identityByte != 0xff) 
				memory_write((_DEVICE_INDEX__ + i), identityByte);
			
			if (address >= _DEVICE_ADDRESS_END__) return;
			address += 0x10000;
		}
		
		return;
	}
	
	// Get a device address by its identity byte
	uint32_t getDeviceAddress(uint8_t identity_byte) {
		
		uint32_t address=_DEVICE_ADDRESS_START__;
		
		for (uint8_t i=0; i < 32; i++) {
			
			char readIdentityByte=0x00;
			memory_read((_DEVICE_INDEX__ + i), readIdentityByte);
			
			if (identity_byte == readIdentityByte) return address;
			
			address += 0x10000;
			if (address > _DEVICE_ADDRESS_END__) break;
		}
		
		return 0x00000;
	}
	
};
DeviceHandler deviceHandler;


