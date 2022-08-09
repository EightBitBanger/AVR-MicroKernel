//
// On-chip EEPROM storage driver

#ifndef _EEPROM_DRIVER__
#define _EEPROM_DRIVER__

void EEPROMDeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct EEPROMDriverLoader {
	
	EEPROMDriverLoader() {
		load_device(_DATA_MEMORY__, sizeof(_DATA_MEMORY__), (Device)EEPROMDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
	}
	
};EEPROMDriverLoader eepromDriverLoader;


struct EEPROMStorage {
	
	WrappedPointer pointer;
	
	void initiate(void) {
		
	}
	
	void shutdown(void) {
		
	}
	
	
	void write(uint32_t address, char byte) {
		// Wait for completion of previous write
		while(EECR & (1<<EEPE));
		
		// Set up address and Data Registers
		EEAR = address;
		EEDR = byte;
		
		// Write logical one to EEMPE
		EECR |= (1<<EEMPE);
		// Start EEPROM write by setting EEPE
		EECR |= (1<<EEPE);
	}
	
	void read(uint32_t address, char& byte) {
		// Wait for completion of previous write
		while(EECR & (1<<EEPE));
		
		// Set up address register
		EEAR = address;
		// Start EEPROM read by writing EERE
		EECR |= (1<<EERE);
		
		// Return data from Data Register
		byte = EEDR;
	}
	
}static eepromStorage;




void EEPROMDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	if (functionCall == DEVICE_CALL_INITIATE) {eepromStorage.initiate(); return;}
	if (functionCall == DEVICE_CALL_SHUTDOWN) {eepromStorage.shutdown(); return;}
	if (functionCall == DEVICE_CALL_ADDRESS) {eepromStorage.pointer.byte_t[0] = paramA; eepromStorage.pointer.byte_t[1] = paramB; eepromStorage.pointer.byte_t[2] = paramC; eepromStorage.pointer.byte_t[3] = paramD; return;}
	
	if (functionCall == 0x01) {eepromStorage.write(eepromStorage.pointer.address, paramA); return;}
	if (functionCall == 0x02) {eepromStorage.read(eepromStorage.pointer.address, (char&)paramA); return;}
	
	// Auto increment Read / Write
	if (functionCall == 0x03) {eepromStorage.write(eepromStorage.pointer.address, paramA);       eepromStorage.pointer.address++; return;}
	if (functionCall == 0x04) {eepromStorage.read(eepromStorage.pointer.address, (char&)paramA); eepromStorage.pointer.address++; return;}
	
	return;
}




#endif

