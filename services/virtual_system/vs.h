
void intiate_virtual_system(void) {
	
	Bus memorybus;
	memorybus.waitstate_read  = 2;
	memorybus.waitstate_write = 0;
	
	// Zero the memory
	for (uint32_t i=_VIRTUAL_STORAGE_ADDRESS__; i < (_VIRTUAL_STORAGE_ADDRESS__ + _VIRTUAL_STORAGE_SIZE__); i++) {
		bus_write_byte(memorybus, i, 0x00);
	}
	
	// Initiate sector header
	char header[] = {0x13,'s','t','o','r','a','g','e',' ',' ',  0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00};
	
	for (uint32_t i=_VIRTUAL_STORAGE_ADDRESS__; i < (_VIRTUAL_STORAGE_ADDRESS__ + sizeof(header)); i++) {
		bus_write_byte(memorybus, i, header[i]);
	}
	
	// Setup virtual device files
	uint8_t current_prompt = console.promptString[0];
	console.promptString[0] = '/';
	
	char filename[] = "config ";
	fs.file_create(filename, 1);
	
	char port_a[] = "porta";
	fs.file_create(port_a, 1);
	char port_b[] = "portb";
	fs.file_create(port_b, 1);
	
	// Return the old prompt
	console.promptString[0] = current_prompt;
	
	return;
}

