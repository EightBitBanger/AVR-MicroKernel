// EEPROM hard storage driver

#ifndef _HARD_STORAGE_DRIVER__
#define _HARD_STORAGE_DRIVER__

void HardStorageDeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct StorageDriverLoader {
	StorageDriverLoader() {loadLibrary(_HARD_STORAGE__, sizeof(_HARD_STORAGE__), (DriverEntryPoint)HardStorageDeviceDriverEntryPoint);}
};StorageDriverLoader storageDriverLoader;


struct HardStorage {
	
	WrappedPointer pointer;
	
	void initiate(void) {
		
	}
	
	void shutdown(void) {
		
	}
	
	
	void write(uint32_t address, char data_byte) {
		// Wait for completion of previous write
		while(EECR & (1<<EEPE));
		
		// Set up address and Data Registers
		EEAR = address;
		EEDR = data_byte;
		
		// Write logical one to EEMPE
		EECR |= (1<<EEMPE);
		// Start EEPROM write by setting EEPE
		EECR |= (1<<EEPE);
	}
	
	void read(uint32_t address, char& data_byte) {
		// Wait for completion of previous write
		while(EECR & (1<<EEPE));
		
		// Set up address register
		EEAR = address;
		// Start EEPROM read by writing EERE
		EECR |= (1<<EERE);
		
		// Return data from Data Register
		data_byte = EEDR;
	}
	
} static hardStorage;





void HardStorageDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	switch(functionCall) {
		
		case _DRIVER_INITIATE__: {hardStorage.initiate(); break;}
		case _DRIVER_SHUTDOWN__: {hardStorage.shutdown(); break;}
		
		case 0x00:  hardStorage.pointer.byte_t[0] = paramA; hardStorage.pointer.byte_t[1] = paramB; hardStorage.pointer.byte_t[2] = paramC; hardStorage.pointer.byte_t[3] = paramD; break;
		
		case 0x01:  hardStorage.write(hardStorage.pointer.address, paramA); break;
		case 0x02:  hardStorage.read(hardStorage.pointer.address, (char&)paramA); break;
		
		// Auto increment Read / Write
		case 0x03:  hardStorage.write(hardStorage.pointer.address, paramA);       hardStorage.pointer.address++; break;
		case 0x04:  hardStorage.read(hardStorage.pointer.address, (char&)paramA); hardStorage.pointer.address++; break;
		
		default: break;
	}
	
	return;
}




#endif

