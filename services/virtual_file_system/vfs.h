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
	
	char port_a[] = "port_a ";
	file_create(port_a, 32, attr_normal);
	
	char port_b[] = "port_b ";
	file_create(port_b, 32, attr_normal);
	
	
	char asm_version[] = {
		
		0xa0, rAX, 0x00,
		0xa0, rDX, 'A', 0xcd, 0x10,
		0xa0, rDX, 'V', 0xcd, 0x10,
		0xa0, rDX, 'R', 0xcd, 0x10,
		0xa0, rDX, '-', 0xcd, 0x10,
		0xa0, rDX, 'K', 0xcd, 0x10,
		0xa0, rDX, 'e', 0xcd, 0x10,
		0xa0, rDX, 'r', 0xcd, 0x10,
		0xa0, rDX, 'n', 0xcd, 0x10,
		0xa0, rDX, 'e', 0xcd, 0x10,
		0xa0, rDX, 'l', 0xcd, 0x10,
		0xa0, rDX, ' ', 0xcd, 0x10,
		0xa0, rDX, '1', 0xcd, 0x10,
		0xa0, rDX, '.', 0xcd, 0x10,
		0xa0, rDX, '0', 0xcd, 0x10,
		
		0xa0, rAX, 0x01, 0xcd, 0x10,
		
		0xcd, 0x20,
		
	};
	
	char sys_program[] = "ver ";
	file_create(sys_program, sizeof(asm_version), attr_executable);
	
	if (file_open(sys_program) == 1) {
		
		for (uint8_t a=0; a < sizeof(asm_version); a++) 
			file_write_byte(a, asm_version[a]);
		
		file_close();
		
	}
	
	
	
	
	char asm_net_send[] = {
		
		0xa0,0x00,0x55,
		0xa0,0x01,0x55,
		0xa0,0x02,0x00,
		0xa0,0x03,0xaa,  0xcd,0x14,
		
		0xcd,0x20,
		
	};
	
	char sys_net_send[] = "send ";
	file_create(sys_net_send, sizeof(asm_net_send), attr_executable);
	
	if (file_open(sys_net_send) == 1) {
		
		for (uint8_t a=0; a < sizeof(asm_net_send); a++)
		file_write_byte(a, asm_net_send[a]);
		
		file_close();
		
	}
	
	
	
	
	// Return the old prompt
	console.promptString[0] = current_prompt;
	
	return;
}

