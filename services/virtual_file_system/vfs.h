//
// Virtual storage device (ram-drive)

void intiate_virtual_system(void) {
	
	Bus memorybus;
	memorybus.waitstate_read  = 2;
	memorybus.waitstate_write = 0;
	
	// Zero the memory and format the virtual disk
	for (uint32_t i=_VIRTUAL_STORAGE_ADDRESS__; i < (_VIRTUAL_STORAGE_ADDRESS__ + _VIRTUAL_STORAGE_SIZE__); i++) 
		bus_write_byte(memorybus, i, 0x20);
	for (uint32_t i=_VIRTUAL_STORAGE_ADDRESS__; i < (_VIRTUAL_STORAGE_ADDRESS__ + _VIRTUAL_STORAGE_SIZE__); i += SECTOR_SIZE) 
		bus_write_byte(memorybus, i, 0x00);
	
	// Initiate sector header
	char header[] = {0x13,'s','t','o','r','a','g','e',' ',' ',  0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00};
	
	for (uint32_t i=_VIRTUAL_STORAGE_ADDRESS__; i < (_VIRTUAL_STORAGE_ADDRESS__ + sizeof(header)); i++) {
		bus_write_byte(memorybus, i, header[i]);
	}
	
	return;
}

