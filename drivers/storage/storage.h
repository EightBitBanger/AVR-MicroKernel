//
// Storage and file system driver

#ifndef _DRIVER_MASS_STORAGE__
#define _DRIVER_MASS_STORAGE__

void storageDeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

char msg_file_count[] = "files";

struct MassStorageDeviceDriver {
	
	Bus device_bus;
	
	uint32_t device_address;
	uint8_t  page_count;
	
	MassStorageDeviceDriver() {
		
		device_address = 0x00000;
		
		device_bus.waitstate_read  = 40;
		device_bus.waitstate_write = 100;
		
		page_count = 0;
		
		load_device(_MASS_STORAGE__, sizeof(_MASS_STORAGE__), (Module)storageDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
	}
	
	void initiate(void) {
		
	}
	
	void shutdown(void) {
		
	}
	
	void write(uint32_t address, char byte) {bus_write_byte(device_bus, address, byte); return;}
	
	void read(uint32_t address, char& byte) {bus_read_byte(device_bus, address, byte); return;}
	
	void list_directory(uint8_t enable_page_pause) {
		
		WrappedPointer pointer;
		uint8_t page_counter=0;
		char byte;
		
		// List current device contents
		uint32_t current_device = 0x30000 + (0x10000 * (console.promptString[0] - 'A' + 1));
		uint8_t  stride         = 32;
		uint32_t device_start   = current_device + stride;
		uint32_t device_end     = current_device + (1024 * 8);
		
		uint16_t file_count=0;
		
		for (uint32_t i=device_start; i < device_end; i += stride) {
			
			read(i, byte);
			
			if (byte == 0xff) continue; // Ignore file data sectors
			if (byte == 0xaa) continue; // Ignore file end sectors
			if (byte != 0x55) continue; // Only file header sectors
			
			file_count++;
			
			// Display file name
			for (uint8_t a=0; a < 10; a++) {
				read(i + a + 1, byte);
				console.printChar(byte);
			}
			
			// Display file size
			WrappedPointer filesize;
			for (uint8_t a=0; a < 4; a++) 
				read(i + a + 11, (char&)filesize.byte_t[a]);
			
			console.printSpace();
			console.printInt(filesize.address);
			
			page_counter++;
			console.printLn();
			
			// Page pause
			if (enable_page_pause == 1) if (page_counter > 2) {page_counter = 0; console.pause_press_anykey();}
			
		}
		
		// Display total file count
		console.printSpace();
		console.printInt(file_count);
		console.printSpace();
		if (file_count > 10) console.printSpace();
		if (file_count > 100) console.printSpace();
		
		if (file_count == 1) {
			console.print(msg_file_count, sizeof(msg_file_count) - 1);
		} else {
			console.print(msg_file_count, sizeof(msg_file_count));
		}
		console.printLn();
		
		// Page pause
		if (enable_page_pause == 1) if (page_counter > 2) {page_counter = 0; console.pause_press_anykey();}
		
		return;
	}
	
	uint8_t file_create(char* file_name, uint32_t file_size) {
		
		WrappedPointer pointer;
		char byte;
		
		uint32_t current_device = 0x30000 + (0x10000 * (console.promptString[0] - 'A' + 1));
		
		uint32_t device_start        = current_device;
		uint32_t device_end          = current_device + (32 * 256);   // 8k
		
		uint32_t total_sectors       = device_end / 32;
		uint32_t file_sector_size    = file_size;
		uint32_t count_free_sectors  = 0;
		
		for (uint32_t i=device_start; i < device_end; i += 32) {
			
			read(i, byte);
			
			// Check sector is empty
			if (byte != 0x00) continue;
			
			// Check next bytes for requested file size
			for (uint32_t a=1; a < total_sectors; a++) {
				
				read(i + (a * 32), byte);
				
				if (byte == 0x00) {
					count_free_sectors++;
					
					if (count_free_sectors >= file_sector_size) {
						count_free_sectors = file_sector_size;
						break;
					}
					
					continue;
				}
				
				break;
			}
			
			// File cannot fit into this empty space, try again
			if (file_sector_size != count_free_sectors)
				continue;
			
			
			// File size is available, mark sector as taken
			byte = 0x55; // File start byte
			write(i, byte);
			_delay_ms(5);
			
			// Write file name
			for (uint32_t a=0; a < 10; a++) {
				write(i + a + 1, (uint8_t&)file_name[a]);
				_delay_ms(5);
			}
			
			// Write file size
			WrappedPointer filesize;
			filesize.address = file_size;
			if (filesize.address == 0) filesize.address = 1;
			for (uint32_t a=0; a < 4; a++) {
				write(i + a + 11, (uint8_t&)filesize.byte_t[a]);
				_delay_ms(5);
			}
			
			uint32_t number_of_sectors = (file_size / 32) + 1;
			if (number_of_sectors <= 2) number_of_sectors = 2;
			
			// Mark following sectors as taken
			byte = 0xff;
			for (uint32_t a=1; a < number_of_sectors; a++) {
				
				if (a == (number_of_sectors-1))
					byte = 0xaa;
				
				write(i + (a * 32), byte);
				_delay_ms(5);
				
			}
			
			return 1;
		}
		
		return 0;
	}
	
	uint32_t file_get_size(char* file_name) {
		
		WrappedPointer pointer;
		char byte;
		
		// List current device contents
		uint32_t current_device = 0x30000 + (0x10000 * (console.promptString[0] - 'A' + 1));
		uint8_t  stride         = 32;
		uint32_t device_start   = current_device + 32;
		uint32_t device_end     = current_device + (1024 * 8);
		
		for (uint32_t i=device_start; i < device_end; i += stride) {
			
			read(i, byte);
			
			// Check sector header byte
			if (byte != 0x00) {
				
				if (byte == 0xff) continue; // Ignore file data sectors
				if (byte == 0xaa) continue; // Ignore file end sectors
				
				// Get file name
				char current_file_name[11] = "          ";
				for (uint32_t a=1; a < 10; a++) 
					read(i + a, (char&)current_file_name[a-1]);
				
				// Compare filenames
				if (strcmp(current_file_name, file_name, 10) == 1) {
					
					// Get file size
					WrappedPointer filesize;
					for (uint32_t a=0; a < 4; a++) 
						read(i + a + 11, (char&)filesize.byte_t[a]);
					
					return filesize.address;
				}
				
			}
			
		}
		
		return 0;
	}
	
	
	uint8_t file_delete(char* file_name) {
		
		WrappedPointer pointer;
		char byte;
		
		// List current device contents
		uint32_t current_device = 0x30000 + (0x10000 * (console.promptString[0] - 'A' + 1));
		uint8_t  stride         = 32;
		uint32_t device_start   = current_device + 32;
		uint32_t device_end     = current_device + (1024 * 8);
		
		uint8_t page_counter=0;
		
		for (uint32_t i=device_start; i < device_end; i += stride) {
			
			read(i, byte);
			
			// Check sector header byte
			if (byte != 0x00) {
				
				if (byte == 0xff) continue; // Ignore file data sectors
				if (byte == 0xaa) continue; // Ignore file end sectors
				
				// Get files name
				char current_file_name[11] = "          ";
				for (uint32_t a=1; a < 10; a++)
				read(i + a, (char&)current_file_name[a-1]);
				
				// Compare filenames
				if (strcmp(current_file_name, file_name, 10) == 1) {
					
					// Get file size
					WrappedPointer filesize;
					for (uint32_t a=0; a < 4; a++)
					read(i + a + 11, (char&)filesize.byte_t[a]);
					
					uint32_t number_of_sectors = (filesize.address / 32) + 1;
					
					// Zero the file sectors
					byte = 0x00;
					for (uint32_t a=0; a < number_of_sectors; a++) {
						write(i + (a * stride), byte);
						_delay_ms(5);
					}
					
					return 1;
				}
				
			}
			
		}
		
		return 0;
	}
	
	
	void file_write_byte(void) {
		
	}
	
	
	void file_read_byte(void) {
		
	}
	
	
}static fs;


void storageDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	if (functionCall == DEVICE_CALL_INITIATE) {fs.initiate(); return;}
	if (functionCall == DEVICE_CALL_SHUTDOWN) {fs.shutdown(); return;}
	if (functionCall == DEVICE_CALL_ADDRESS) {
		WrappedPointer pointer; pointer.byte_t[0] = paramA; pointer.byte_t[1] = paramB; pointer.byte_t[2] = paramC; pointer.byte_t[3] = paramD;
		fs.device_address = pointer.address;
		return;
	}
	
	if (functionCall == 0x00) {fs.read(fs.device_address, (char&)paramA); return;}
	if (functionCall == 0x01) {fs.write(fs.device_address, (char)paramA); return;}
	
	return;
}


#undef _HARDWARE_WAITSTATE__












#endif

