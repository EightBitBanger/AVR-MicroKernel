//
// File system device driver

#ifndef _DRIVER_MASS_STORAGE__
#define _DRIVER_MASS_STORAGE__

#define SECTOR_SIZE  32

#define FILE_ATTRIBUTE_EXECUTEABLE  0xf0

#define DEVICE_CAPACITY  (SECTOR_SIZE * 256)    // 8,192 bytes
//#define DEVICE_CAPACITY  (SECTOR_SIZE * 512)    // 16,384 bytes
//#define DEVICE_CAPACITY  (SECTOR_SIZE * 1024)   // 32,768 bytes

#define _VIRTUAL_STORAGE_ADDRESS__    0x30000
#define _VIRTUAL_STORAGE_SIZE__       0x10000

const char msg_file_count[]            = "files";
const char msg_file_not_found[]        = "File not found.";
const char msg_device_not_found[]      = "Device not found.";
const char msg_file_already_exists[]   = "File already exists.";
const char msg_file_write_protected[]  = "File write protected.";
const char msg_device_not_ready[]      = "Device not ready...";

// Creates a new file on the file system
uint8_t file_create(char* filename, uint32_t file_size, uint8_t* attributes);
// Removes a file from the file system
uint8_t file_delete(char* filename);
// Renames a file to a new name
uint8_t file_rename(char* filename, char* new_file_name);
// Gets the size of the named file
uint32_t file_get_size(char* filename);

// Returns a files attributes
uint8_t file_get_attribute(char* filename, uint8_t position);
// Sets a files attributes
uint8_t file_set_attribute(char* filename, uint8_t attribute, uint8_t position=0);

// Opens a file for IO
uint8_t file_open(char* filename);
// Reads a byte from the currently open file
uint8_t file_read_byte(uint32_t address, char& byte);
// Writes a byte to the currently open file
uint8_t file_write_byte(uint32_t address, char byte);
// Closes the currently open file
uint8_t file_close(void);

// Wait state for EEPROM write cycles
void eeprom_wait_state(void);
// Print the files on the current device
void list_directory(uint8_t enable_page_pause);
// Sets the current device scope from the console prompt string
uint32_t set_device_scope(void);


struct MassStorageDeviceDriver {
	
	Bus device_bus;
	
	char file_name[10];
	
	uint32_t device_address;
	
	// File state
	uint32_t file_address;
	uint32_t file_size;
	
	MassStorageDeviceDriver() {
		
		device_address = 0x00000;
		
		for (uint8_t i=0; i < sizeof(file_name); i++) 
			file_name[i]=0x20;
		
		device_bus.waitstate_read  = 2;
		device_bus.waitstate_write = 0;
		
		file_address = 0;
		file_size    = 0;
		
	}
	
	void write(uint32_t address, char byte) {bus_write_byte(device_bus, address, byte); return;}
	void read(uint32_t address, char& byte) {bus_read_byte(device_bus, address, byte); return;}
	
	// Check a storage device header
	uint8_t device_check_header(uint32_t current_device) {
		
		HardwareInformation hInfo;
		get_hardware_info(current_device, device_bus, hInfo);
		
		if (hInfo.device_id == 0x13) {
			
			if (strcmp(hInfo.device_name, _MASS_STORAGE__, sizeof(_MASS_STORAGE__)) == 1) {
				return 1;
			}
			
		}
		return 0;
	}
	
}static fs;



void list_directory(uint8_t enable_page_pause) {
	
	uint8_t page_counter=0;
	char byte;
	
	// Link to the console driver to list the directory
	Device consoleDevice = (Device)get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__));
	
	uint32_t current_device = set_device_scope();
	
	if (fs.device_check_header(current_device - SECTOR_SIZE) == 0) {
		console.print(msg_device_not_ready, sizeof(msg_device_not_ready));
		console.printLn();
		return;
	}
	
	uint32_t device_start   = current_device;
	uint32_t device_end     = current_device + DEVICE_CAPACITY;
	
	uint16_t file_count=0;
	
	for (uint32_t i=device_start; i < device_end; i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		if (byte != 0x55) continue; // Only file header sectors
		
		file_count++;
		
		// Display file name
		for (uint8_t a=0; a < 10; a++) {
			fs.read(i + a + 1, byte);
			call_extern(consoleDevice, 0x00, (uint8_t&)byte);
		}
		
		call_extern(consoleDevice, 0x03); // Space
		
		// Display file size
		WrappedPointer filesize;
		for (uint8_t a=0; a < 4; a++)
			fs.read(i + a + 11, (char&)filesize.byte_t[a]);
		
		// Scale the file size
		filesize.address = filesize.address * SECTOR_SIZE;
		
		// Check > thousand bytes
		if (filesize.address > 999) {
			
			if (filesize.address < 9999)
				call_extern(consoleDevice, 0x03); // Space
			
			WrappedPointer tsize;
			tsize.address = (filesize.address / 1000);
			
			call_extern(consoleDevice, 0x04, tsize.byte_t[0], tsize.byte_t[1], tsize.byte_t[2], tsize.byte_t[3]);
			
			// k Thousand bytes
			uint8_t thousand_char = 'k';
			call_extern(consoleDevice, 0x00, thousand_char);
			call_extern(consoleDevice, 0x03); // Space
			
		} else {
			
			if (filesize.address < 100)
				call_extern(consoleDevice, 0x03); // Space
			if (filesize.address < 10)
				call_extern(consoleDevice, 0x03); // Space
			
			call_extern(consoleDevice, 0x04, filesize.byte_t[0], filesize.byte_t[1], filesize.byte_t[2], filesize.byte_t[3]);
			call_extern(consoleDevice, 0x03); // Space
			
		}
		
		// Display file attributes
		uint8_t attribute;
		for (uint8_t a=0; a < 4; a++) {
			
			fs.read(i + a + 15, (char&)attribute);
			
			if (attribute < 0x61) attribute = 0x20;
				call_extern(consoleDevice, 0x00, (uint8_t&)attribute);
		}
		
		page_counter++;
		call_extern(consoleDevice, 0x01); // New line
		
		// Page pause
		if (enable_page_pause == 1) if (page_counter > 2) {page_counter = 0; console.pause_press_anykey();}
		
	}
	
	return;
}


uint8_t file_create(char* filename, uint32_t file_size, uint8_t* attributes) {
	
	// Check storage device
	uint32_t current_device = set_device_scope();
	
	uint32_t device_start   = current_device;
	uint32_t device_end     = current_device + DEVICE_CAPACITY;
	
	uint32_t count_free_sectors  = 0;
	
	char byte;
	
	// Clean up the filename
	char file_name[10];
	for (uint8_t a=0; a < 10; a++)
		file_name[a] = 0x20;
	
	for (uint8_t a=0; a < 10; a++) {
		if (filename[a] == ' ') break;
		file_name[a] = filename[a];
	}
	
	// File sector count
	uint32_t number_of_sectors = 1 + (file_size / (SECTOR_SIZE - 1));
	
	for (uint32_t i=device_start; i < device_end; i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		// Continue until free space
		if (byte != 0x00) continue;
		
		// Check the following sectors for required total free space
		for (uint32_t a=i; a < device_end; a+=SECTOR_SIZE) {
			
			fs.read(a, byte);
			
			if (byte == 0x00) {
				
				if (count_free_sectors == number_of_sectors)
					break;
				
				count_free_sectors++;
				
				continue;
			}
			
			count_free_sectors=0;
			break;
		}
		
		// File cannot fit into this empty space, continue seeking free space
		if (number_of_sectors != count_free_sectors)
			continue;
		
		
		// File size is available
		// Mark first sector as a file header sector
		byte = 0x55; // File start byte 0x55
		fs.write(i, byte); eeprom_wait_state();
		
		// Clear file name
		for (uint8_t a=0; a <= 10; a++) {
			fs.write(i + a + 1, 0x20); eeprom_wait_state();
		}
		
		// Write file name
		for (uint8_t a=0; a < 10; a++) {
			fs.write(i + a + 1, (uint8_t&)file_name[a]); eeprom_wait_state();
			if (file_name[a] == 0x20) break;
		}
		
		// Write file size
		WrappedPointer file_size_ptr;
		file_size_ptr.address = file_size / SECTOR_SIZE;
		if (file_size_ptr.address == 0) file_size_ptr.address = 1;
		for (uint8_t a=0; a < 4; a++) {
			fs.write(i + a + 11, (uint8_t&)file_size_ptr.byte_t[a]); eeprom_wait_state();
		}
		
		// Clear attributes
		byte = 0x00;
		for (uint8_t a=0; a < 4; a++) {
			fs.write(i + 15 + a, (uint8_t&)byte); eeprom_wait_state();
		}
		
		// Set file attributes
		fs.write(i + 15, attributes[0]); eeprom_wait_state();
		fs.write(i + 15 + 1, attributes[1]); eeprom_wait_state();
		fs.write(i + 15 + 2, attributes[2]); eeprom_wait_state();
		fs.write(i + 15 + 3, attributes[3]); eeprom_wait_state();
		
		// Mark following sectors as "taken"
		byte = 0xff; // File taken sectors 0xff
		for (uint32_t a=1; a < (number_of_sectors + 1); a++) {
			
			// Mark last sector as "file end"
			if (a == (number_of_sectors))
				byte = 0xaa; // File end sector 0xaa
			
			fs.write(i + (a * SECTOR_SIZE), byte); eeprom_wait_state();
			
		}
		
		return 1;
	}
	
	return 0;
}


uint8_t file_delete(char* filename) {
	
	// Check storage device
	uint32_t current_device = set_device_scope();
	
	uint32_t device_start   = current_device;
	uint32_t device_end     = current_device + DEVICE_CAPACITY;
	
	char byte;
	
	// Clean up the filename
	char file_name[10];
	for (uint8_t a=0; a < 10; a++)
		file_name[a] = 0x20;
	
	for (uint8_t a=0; a < 10; a++) {
		if (filename[a] == ' ') break;
		file_name[a] = filename[a];
	}
	
	for (uint32_t i=device_start; i < device_end; i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		// Check sector header byte
		if (byte == 0x55) {
			
			char current_file_name[10];
			for (uint8_t a=0; a < 10; a++)
				current_file_name[a] = 0x20;
			
			// Get files name
			for (uint8_t a=0; a < 10; a++)
				fs.read(i + a + 1, (char&)current_file_name[a]);
			
			// Compare filenames
			if (strcmp(current_file_name, file_name, 10) == 1) {
				
				// Zero the file sectors
				for (uint16_t a=0; a < 100000; a++) {
					
					fs.read(i + (a * SECTOR_SIZE), byte);
					
					if (byte == 0x55) {
						byte = 0x00;
						fs.write(i + (a * SECTOR_SIZE), byte); eeprom_wait_state();
						continue;
					}
					
					if (byte == 0xff) {
						byte = 0x00;
						fs.write(i + (a * SECTOR_SIZE), byte); eeprom_wait_state();
						continue;
					}
					
					if (byte == 0xaa) {
						byte = 0x00;
						fs.write(i + (a * SECTOR_SIZE), byte); eeprom_wait_state();
						break;
					}
					
					break;
				}
				
				return 1;
			}
			
		}
		
	}
	
	return 0;
}


uint8_t file_rename(char* filename, char* new_file_name) {
	
	// Check storage device
	uint32_t current_device = set_device_scope();
	
	uint32_t device_start   = current_device;
	uint32_t device_end     = current_device + DEVICE_CAPACITY;
	
	uint8_t attribute[4];
	char byte;
	
	// Clean up the filename
	char file_name[10];
	for (uint8_t a=0; a < 10; a++)
		file_name[a] = 0x20;
	
	for (uint8_t a=0; a < 10; a++) {
		if (filename[a] == ' ') break;
		file_name[a] = filename[a];
	}
	
	for (uint32_t i=device_start; i < device_end; i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		if (byte != 0x55) continue; // Only file header sectors
		
		// Get current filename
		char current_file_name[16];
		for (uint8_t a=0; a < 16; a++)
			current_file_name[a] = 0x20;
		
		for (uint8_t a=0; a < 10; a++)
			fs.read(i + a + 1, current_file_name[a]);
		
		// Compare filenames
		if (strcmp(current_file_name, file_name, 10) == 1) {
			
			// Write new file name
			for (uint8_t a=0; a < 10; a++) {
				fs.write(i + a + 1, (uint8_t&)new_file_name[a]); eeprom_wait_state();
				
				if (new_file_name[a] == 0x20) break;
			}
			
			return 1;
		}
		
	}
	
	return 0;
}



uint8_t file_open(char* filename) {
	
	// Check storage device
	uint32_t current_device = set_device_scope();
	
	uint32_t device_start   = current_device;
	uint32_t device_end     = current_device + DEVICE_CAPACITY;
	
	char byte;
	
	// Clean up the filename
	char file_name[10];
	for (uint8_t a=0; a < 10; a++)
		file_name[a] = 0x20;
	
	for (uint8_t a=0; a < 10; a++) {
		if (filename[a] == ' ') break;
		file_name[a] = filename[a];
	}
	
	for (uint32_t i=device_start; i < device_end; i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		if (byte != 0x55) continue; // Only file header sectors
		
		// Get current filename
		char current_file_name[16];
		for (uint8_t a=0; a < 16; a++)
			current_file_name[a] = 0x20;
		
		for (uint8_t a=0; a < 10; a++)
			fs.read(i + a + 1, current_file_name[a]);
		
		// Compare filenames
		if (strcmp(current_file_name, file_name, 10) == 1) {
			
			// Get file size
			WrappedPointer filesize;
			for (uint8_t a=0; a < 4; a++)
				fs.read(i + a + 11, (char&)filesize.byte_t[a]);
			
			// Open the file
			fs.file_address = i;
			fs.file_size    = filesize.address * SECTOR_SIZE;
			
			return 1;
		}
		
	}
	return 0;
}



uint8_t file_close(void) {
	fs.file_address=0;
}


uint32_t file_get_size(char* filename) {
	
	// Check storage device
	uint32_t current_device = set_device_scope();
	
	uint32_t device_start   = current_device;
	uint32_t device_end     = current_device + DEVICE_CAPACITY;
	
	uint8_t attribute[4];
	char byte;
	
	for (uint32_t i=device_start; i < device_end; i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		if (byte != 0x55) continue; // Only file header sectors
		
		// Get current filename
		char current_file_name[16];
		for (uint8_t a=0; a < 16; a++)
			current_file_name[a] = 0x20;
		
		for (uint8_t a=0; a < 10; a++)
			fs.read(i + a + 1, current_file_name[a]);
		
		// Compare filenames
		if (strcmp(current_file_name, filename, 10) == 1) {
			
			// Get file size
			WrappedPointer filesize;
			for (uint8_t a=0; a < 4; a++)
				fs.read(i + a + 11, (char&)filesize.byte_t[a]);
			
			return (filesize.address * SECTOR_SIZE);
		}
		
	}
	
	return 0;
}



uint8_t file_set_attribute(char* filename, uint8_t attribute, uint8_t position) {
	
	// Check storage device
	uint32_t current_device = set_device_scope();
	
	uint32_t device_start   = current_device;
	uint32_t device_end     = current_device + DEVICE_CAPACITY;
	
	uint8_t attributes[4];
	char byte;
	
	for (uint32_t i=device_start; i < device_end; i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		if (byte != 0x55) continue; // Only file header sectors
		
		// Get current filename
		char current_file_name[16];
		for (uint8_t a=0; a < 16; a++)
			current_file_name[a] = 0x20;
		
		for (uint8_t a=0; a < 10; a++)
			fs.read(i + a + 1, current_file_name[a]);
		
		// Compare filenames
		if (strcmp(current_file_name, filename, 10) == 1) {
			
			// Read current file attributes
			for (uint8_t a=0; a < 4; a++)
				fs.read(i + a + 15, (char&)attributes[a]);
			
			attributes[position] = attribute;
			
			// Write back file attributes
			for (uint8_t a=0; a < 4; a++) {
				fs.write(i + a + 15, (char&)attributes[a]); eeprom_wait_state();
			}
			
			return 1;
		}
		
	}
	
	return 0;
}


uint8_t file_get_attribute(char* filename, uint8_t position) {
	
	// Check storage device
	uint32_t current_device = set_device_scope();
	
	uint32_t device_start   = current_device;
	uint32_t device_end     = current_device + DEVICE_CAPACITY;
	
	uint8_t attributes[4];
	char byte;
	
	for (uint32_t i=device_start; i < device_end; i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		if (byte != 0x55) continue; // Only file header sectors
		
		// Get current filename
		char current_file_name[16];
		for (uint8_t a=0; a < 16; a++)
			current_file_name[a] = 0x20;
		
		for (uint8_t a=0; a < 10; a++)
			fs.read(i + a + 1, current_file_name[a]);
		
		// Compare filenames
		if (strcmp(current_file_name, filename, 10) == 1) {
			
			// Read current file attributes
			for (uint8_t a=0; a < 4; a++) {
				fs.read(i + a + 15, (char&)attributes[a]);
				if (attributes[a] < 0x61) attributes[a] = 0x20;
			}
			
			return attributes[position];
		}
		
	}
	
	return 0;
}


uint8_t file_write_byte(uint32_t address, char byte) {
	if (fs.file_address == 0) return 0;
	address += ((address / (SECTOR_SIZE - 1)) + 1) + SECTOR_SIZE;
	if ((fs.file_address + address) > (fs.file_address + (fs.file_size * SECTOR_SIZE))) return 0;
	fs.write(fs.file_address + address, byte); eeprom_wait_state();
	return 1;
}


uint8_t file_read_byte(uint32_t address, char& byte) {
	if (fs.file_address == 0) return 0;
	address += (address / (SECTOR_SIZE - 1) + 1) + SECTOR_SIZE;
	if ((fs.file_address + address) > (fs.file_address + (fs.file_size * SECTOR_SIZE))) return 0;
	fs.read(fs.file_address + address, byte);
	return 1;
}




void eeprom_wait_state(void) {
	if (console.promptString[0] != '/') 
		_delay_ms(5);
}


uint32_t set_device_scope(void) {
	
	uint32_t current_device = 0x30000 + (0x10000 * (console.promptString[0] - 'A' + 1));
	if (console.promptString[0] == '/') current_device = _VIRTUAL_STORAGE_ADDRESS__;
	
	return (current_device + SECTOR_SIZE);
}




void storageDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	if (functionCall == DEVICE_CALL_ADDRESS) {
		WrappedPointer pointer; pointer.byte_t[0] = paramA; pointer.byte_t[1] = paramB; pointer.byte_t[2] = paramC; pointer.byte_t[3] = paramD;
		fs.device_address = pointer.address;
		return;
	}
	
	if (functionCall == 0x00) {fs.file_name[0] = paramA; return;}
	if (functionCall == 0x01) {fs.file_name[1] = paramA; return;}
	if (functionCall == 0x02) {fs.file_name[2] = paramA; return;}
	if (functionCall == 0x03) {fs.file_name[3] = paramA; return;}
	if (functionCall == 0x04) {fs.file_name[4] = paramA; return;}
	if (functionCall == 0x05) {fs.file_name[5] = paramA; return;}
	if (functionCall == 0x06) {fs.file_name[6] = paramA; return;}
	if (functionCall == 0x07) {fs.file_name[7] = paramA; return;}
	if (functionCall == 0x08) {fs.file_name[8] = paramA; return;}
	if (functionCall == 0x09) {fs.file_name[9] = paramA; return;}
	
	if (functionCall == 0x0a) {uint8_t attr[] = " rw "; paramA = file_create(fs.file_name, fs.device_address, attr); return;}
	if (functionCall == 0x0b) {paramA = file_delete(fs.file_name); return;}
	if (functionCall == 0x0c) {
		WrappedPointer pointer;
		pointer.address = file_get_size(fs.file_name);
		paramA = pointer.byte_t[0];
		paramB = pointer.byte_t[1];
		paramC = pointer.byte_t[2];
		paramD = pointer.byte_t[3];
		return;
	}
	
	if (functionCall == 0x0d) {paramA = file_open(fs.file_name); return;}
	if (functionCall == 0x0e) {paramA = file_close(); return;}
	if (functionCall == 0x0f) {file_read_byte(fs.device_address, (char&)paramA); return;}
	if (functionCall == 0x10) {file_write_byte(fs.device_address, (char&)paramA); return;}
	
	if (functionCall == 0x11) {fs.read(fs.device_address, (char&)paramA); return;}
	if (functionCall == 0x12) {fs.write(fs.device_address, (char&)paramA); return;}
	
	if (functionCall == 0x2f) {list_directory(paramA); return;}
	
	return;
}



#undef _HARDWARE_WAITSTATE__











#endif

