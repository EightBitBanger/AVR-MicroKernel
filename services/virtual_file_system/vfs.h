//
// 

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
	
	// Setup virtual device files
	uint8_t current_prompt = console.promptString[0];
	console.promptString[0] = '/';
	
	// Default attributes
	uint8_t attr_normal[]     = " rw ";
	uint8_t attr_executable[] = "xrw ";
	
	char port_a[] = "file_a ";
	file_create(port_a, 64, attr_normal);
	
	char asm_program[] = {
		
		0xa0, 0x00, 'W', 0xcd, 0x10,
		0xa0, 0x00, 'T', 0xcd, 0x10,
		0xa0, 0x00, 'F', 0xcd, 0x10,
		
		0xcd, 0x20,
		
	};
	
	char sys_program[] = "sys ";
	file_create(sys_program, sizeof(asm_program), attr_executable);
	
	if (file_open(sys_program) == 1) {
		
		for (uint8_t a=0; a < sizeof(asm_program); a++) 
			file_write_byte(a, asm_program[a]);
		
		file_close();
		
	}
	
	// Return the old prompt
	console.promptString[0] = current_prompt;
	
	return;
}

