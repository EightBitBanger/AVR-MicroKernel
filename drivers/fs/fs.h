//
// File system library

#ifndef _DRIVER_FILE_SYSTEM__
#define _DRIVER_FILE_SYSTEM__

const char msg_file_not_found[]        = "File not found.";
const char msg_device_not_found[]      = "Device not found.";
const char msg_file_already_exists[]   = "File already exists.";
const char msg_device_not_ready[]      = "Device not ready...";

#define SECTOR_SIZE  32

struct Attribute {
	uint8_t Executable;
	uint8_t Read;
	uint8_t Write;
	uint8_t Extra;
};

// Format a storage device
uint8_t fs_device_format(uint32_t device_address, uint32_t device_size);
// Get the size of a storage device
uint32_t fs_device_get_size(uint32_t device_address);

// Creates a new file on the file system
uint8_t file_create(char* filename, uint32_t file_size, uint8_t* attributes);
// Removes a file from the file system
uint8_t file_delete(char* filename);
// Renames a file to a new name
uint8_t file_rename(char* filename, char* new_file_name);

// Returns a files attributes
uint8_t file_get_attribute(char* file_name, Attribute& buffer);
// Sets a files attributes
uint8_t file_set_attribute(char* file_name, Attribute& buffer);

// Opens a file
uint8_t file_open(char* filename);
// Reads a byte from the currently open file
uint8_t file_read_byte(uint32_t address, char& byte);
// Writes a byte to the currently open file
uint8_t file_write_byte(uint32_t address, char byte);
// Closes the currently open file
uint8_t file_close(void);

// Sets the current device scope from the console prompt string
uint32_t fs_set_device_scope(void);
// Print the files on the current device
void list_directory(uint8_t enable_page_pause);
// Wait state for EEPROM write cycles
void eeprom_wait_state(void);


struct FileSystemDeviceDriver {
	
	char file_name[10];
	
	uint32_t device_address;
	
	// File state
	uint32_t file_address;
	uint32_t file_size;
	
	Device storageDriverPtr;
	
	FileSystemDeviceDriver() {
		
		device_address = 0x00000;
		
		for (uint8_t i=0; i < sizeof(file_name); i++)
			file_name[i]=0x20;
		
		file_address = 0;
		file_size    = 0;
		
	}
	
	void initiate(void) {
		
		// Link to the storage driver
		storageDriverPtr = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
		if (storageDriverPtr == 0) return;
		
	}
	
	void read(uint32_t address, char& byte) {
		WrappedPointer pointer;
		pointer.address = address;
		call_extern(storageDriverPtr, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		call_extern(storageDriverPtr, 0x00, (uint8_t&)byte);
		return;
	}
	
	void write(uint32_t address, char byte) {
		WrappedPointer pointer;
		pointer.address = address;
		call_extern(storageDriverPtr, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		call_extern(storageDriverPtr, 0x01, (uint8_t&)byte);
		return;
	}
	
}static fs;

uint8_t fs_device_format(uint32_t device_address, uint32_t device_size) {
	
	char flag = 0x00; // Sector state flag
	char byte = 0x20; // Sector data byte
	char chk  = 0x00; // Data check byte
	
	uint8_t sector_counter = 0;
	
	// Wipe the device
	for (uint32_t i=device_address; i < device_address + device_size; i++) {
		if (sector_counter == 32) {sector_counter=0;
			fs.write(i, flag);
		} else {
			fs.write(i, byte);
		}
		sector_counter++;
	}
	
	WrappedPointer size_ptr;
	size_ptr.address = device_size;
	
	// Initiate device header
	char header[] = {0x13, // Hardware ID
					 ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
					 size_ptr.byte[0],size_ptr.byte[1],size_ptr.byte[2],size_ptr.byte[3],
					 0x20,0x20,0x20,0x20,  0x20,0x20,0x20,0x20,
					 0x20,0x20,0x20,0x20,  0x20,0x20,0x20,0x20,
	};
	for (uint8_t i=0; i < sizeof(_FILE_SYSTEM__) - 1; i++)
		header[i + 1] = _FILE_SYSTEM__[i];
	
	// Write the device header
	for (uint8_t i=0; i < sizeof(header); i++) {
		fs.write(i + device_address, header[i]);
	}
	
	return 1;
}

uint32_t fs_device_get_size(uint32_t device_address) {
	WrappedPointer device_size;
	
	if (device_check_header(_FILE_SYSTEM__, sizeof(_FILE_SYSTEM__), device_address) == 0) 
		return 0;
	
	for (uint8_t i=0; i < 4; i++) 
		fs.read(device_address + 12 + i, (char&)device_size.byte_t[i]);
	
	
	return device_size.address;
}



void list_directory(uint8_t enable_page_pause) {
	
	uint8_t page_counter=0;
	char byte;
	
	// Link to the console driver to list the directory
	Device consoleDevice = (Device)get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__));
	
	uint16_t file_count=0;
	
	uint32_t current_device = fs_set_device_scope();
	
	if (device_check_header(_FILE_SYSTEM__, sizeof(_FILE_SYSTEM__), current_device) == 0) {
		console.print(msg_device_not_ready, sizeof(msg_device_not_ready));
		console.printLn();
		return;
	}
	
	uint32_t device_capacity  = fs_device_get_size(current_device);
	uint32_t device_start     = current_device;
	uint32_t device_end       = current_device + device_capacity;
	
	for (uint32_t i=device_start; i < device_end; i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		// Check file sector header
		if (byte != 0x55) continue;
		
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
		filesize.address = filesize.address;
		
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
			call_extern(consoleDevice, 0x00, (uint8_t&)attribute);
		}
		
		page_counter++;
		call_extern(consoleDevice, 0x01); // New line
		
		// Page pause
		if (enable_page_pause == 1) if (page_counter > 2) {page_counter = 0; console.pause_press_anykey();}
		
	}
	
	return;
}


uint8_t file_create(char* file_name, uint32_t file_size, uint8_t* attributes) {
	
	// Check storage device
	uint32_t current_device = fs_set_device_scope();
	if (device_check_header(_FILE_SYSTEM__, sizeof(_FILE_SYSTEM__), current_device) == 0) 
		return 0;
	
	uint32_t device_capacity  = fs_device_get_size(current_device);
	
	uint32_t count_free_sectors  = 0;
	
	char byte;
	
	// File sector count
	uint32_t number_of_sectors=0;
	for (uint32_t i=0; i < file_size; i += (SECTOR_SIZE - 1)) 
		number_of_sectors++;
	
	if (number_of_sectors == 0) 
		number_of_sectors = 1;
	
	for (uint32_t i=current_device; i < current_device + device_capacity; i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		// Continue until free space
		if (byte != 0x00) continue;
		
		// Check the following sectors for required total free space
		for (uint32_t a=i; a < (current_device + device_capacity); a+=SECTOR_SIZE) {
			
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
		fs.write(i, byte);
		
		// Write new file name
		char blank = 0x20;
		uint8_t sw=0;
		for (uint8_t a=0; a < 10; a++) {
			if (sw == 0) {
				fs.write(i + a + 1, file_name[a]);
			} else {
				fs.write(i + a + 1, blank);
			}
			if (file_name[a] == 0x20) sw = 1;
		}
		
		// Write file size
		WrappedPointer file_size_ptr;
		file_size_ptr.address = file_size;
		if (file_size_ptr.address == 0) file_size_ptr.address = 1;
		for (uint8_t a=0; a < 4; a++) {
			fs.write(i + a + 11, file_size_ptr.byte[a]);
		}
		
		// Write attributes
		for (uint8_t a=0; a < 4; a++) {
			fs.write(i + 15 + a, (char&)attributes[a]);
		}
		
		// Mark following sectors as taken
		byte = 0xff; // File taken sectors 0xff
		for (uint32_t a=1; a <= number_of_sectors; a++) {
			
			// Mark last sector as file end
			if (a == number_of_sectors) 
				byte = 0xaa; // File end sector 0xaa
			
			fs.write(i + (a * SECTOR_SIZE), byte);
			
		}
		
		return 1;
	}
	
	return 0;
}


uint8_t file_delete(char* file_name) {
	
	// Check storage device
	uint32_t current_device = fs_set_device_scope();
	if (device_check_header(_FILE_SYSTEM__, sizeof(_FILE_SYSTEM__), current_device) == 0) 
		return 0;
	
	uint32_t device_capacity  = fs_device_get_size(current_device);
	
	char byte;
	char current_file_name[10];
	
	for (uint32_t i=current_device; i < (current_device + device_capacity); i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		// Check sector header byte
		if (byte != 0x55) 
			continue;
		
		// Get files name
		for (uint8_t a=0; a < 10; a++)
			fs.read(i + a + 1, (char&)current_file_name[a]);
		
		// Compare filenames
		if (strcmp(current_file_name, file_name, strln(file_name)) == 1) {
			
			// Zero the file sectors
			for (uint16_t a=0; a < 100000; a++) {
				
				fs.read(i + (a * SECTOR_SIZE), byte);
				
				if (byte == 0x55) {
					byte = 0x00;
					fs.write(i + (a * SECTOR_SIZE), byte);
					continue;
				}
				
				if (byte == 0xff) {
					byte = 0x00;
					fs.write(i + (a * SECTOR_SIZE), byte);
					continue;
				}
				
				if (byte == 0xaa) {
					byte = 0x00;
					fs.write(i + (a * SECTOR_SIZE), byte);
					break;
				}
				
				break;
			}
			
			return 1;
		}
		
		
	}
	
	return 0;
}


uint8_t file_rename(char* file_name, char* new_file_name) {
	
	// Check storage device
	uint32_t current_device = fs_set_device_scope();
	if (device_check_header(_FILE_SYSTEM__, sizeof(_FILE_SYSTEM__), current_device) == 0) 
		return 0;
	
	uint32_t device_capacity  = fs_device_get_size(current_device);
	
	char byte;
	
	for (uint32_t i=current_device; i < (current_device + device_capacity); i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		if (byte != 0x55) continue; // Only file header sectors
		
		// Get current filename
		char current_file_name[16];
		for (uint8_t a=0; a < 16; a++)
			current_file_name[a] = 0x20;
		
		for (uint8_t a=0; a < 10; a++)
			fs.read(i + a + 1, current_file_name[a]);
		
		// Compare filenames
		if (strcmp(current_file_name, file_name, strln(file_name)) == 1) {
			
			// Write new file name
			char blank = 0x20;
			uint8_t sw=0;
			for (uint8_t a=0; a < 10; a++) {
				if (sw == 0) {
					fs.write(i + a + 1, new_file_name[a]);
				} else {
					fs.write(i + a + 1, blank);
				}
				if (new_file_name[a] == 0x20) sw = 1;
			}
			
			return 1;
		}
		
	}
	
	return 0;
}



uint8_t file_open(char* file_name) {
	
	// Check storage device
	uint32_t current_device = fs_set_device_scope();
	if (device_check_header(_FILE_SYSTEM__, sizeof(_FILE_SYSTEM__), current_device) == 0) 
		return 0;
	
	uint32_t device_capacity  = fs_device_get_size(current_device);
	
	char byte;
	
	for (uint32_t i=current_device; i < (current_device + device_capacity); i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		if (byte != 0x55) 
			continue;
		
		char current_file_name[16];
		for (uint8_t a=0; a < 16; a++)
			current_file_name[a] = 0x20;
		
		for (uint8_t a=0; a < 10; a++)
			fs.read(i + a + 1, current_file_name[a]);
		
		if (strcmp(current_file_name, file_name, strln(file_name)) == 1) {
			
			// Get file size
			WrappedPointer filesize;
			for (uint8_t a=0; a < 4; a++)
				fs.read(i + a + 11, (char&)filesize.byte_t[a]);
			
			// Open the file
			fs.file_address = i;
			fs.file_size    = filesize.address;
			
			return 1;
		}
		
	}
	return 0;
}



uint8_t file_close(void) {
	fs.file_address=0;
}




uint8_t file_set_attribute(char* file_name, Attribute& buffer) {
	
	// Check storage device
	uint32_t current_device = fs_set_device_scope();
	if (device_check_header(_FILE_SYSTEM__, sizeof(_FILE_SYSTEM__), current_device) == 0) 
		return 0;
	
	uint32_t device_capacity  = fs_device_get_size(current_device);
	
	uint8_t attributes[4];
	char byte;
	
	for (uint32_t i=current_device; i < (current_device + device_capacity); i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		if (byte != 0x55) 
			continue;
		
		char current_file_name[16];
		for (uint8_t a=0; a < 16; a++)
			current_file_name[a] = 0x20;
		
		for (uint8_t a=0; a < 10; a++)
			fs.read(i + a + 1, current_file_name[a]);
		
		if (strcmp(current_file_name, file_name, strln(file_name)) == 1) {
			
			fs.write(i + 15,     (char&)buffer.Executable);
			fs.write(i + 15 + 1, (char&)buffer.Read);
			fs.write(i + 15 + 2, (char&)buffer.Write);
			fs.write(i + 15 + 3, (char&)buffer.Extra);
			
			return 1;
		}
		
	}
	
	return 0;
}

uint8_t file_get_attribute(char* file_name, Attribute& buffer) {
	
	// Check storage device
	uint32_t current_device = fs_set_device_scope();
	if (device_check_header(_FILE_SYSTEM__, sizeof(_FILE_SYSTEM__), current_device) == 0) 
		return 0;
	
	uint32_t device_capacity  = fs_device_get_size(current_device);
	
	uint8_t attributes[4];
	char byte;
	
	for (uint32_t i=current_device; i < (current_device + device_capacity); i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		if (byte != 0x55) 
			continue;
		char current_file_name[16];
		for (uint8_t a=0; a < 16; a++)
			current_file_name[a] = 0x20;
		
		for (uint8_t a=0; a < 10; a++)
			fs.read(i + a + 1, current_file_name[a]);
		
		if (strcmp(current_file_name, file_name, strln(file_name)) == 1) {
			
			// Read current file attributes
			fs.read(i + 15,     (char&)buffer.Executable);
			fs.read(i + 15 + 1, (char&)buffer.Read);
			fs.read(i + 15 + 2, (char&)buffer.Write);
			fs.read(i + 15 + 3, (char&)buffer.Extra);
			
			return 1;
		}
		
	}
	
	return 0;
}


uint8_t file_write_byte(uint32_t address, char byte) {
	
	if (fs.file_address == 0)
		return 0;
	
	address += address / (SECTOR_SIZE - 1); // Add to the address 1 for each sector header
	address += SECTOR_SIZE;                 // Skip the first sector / header sector
	address ++;                             // Add one to skip the sector state byte
	
	// Check EOF overflow
	if ((fs.file_address + address) > (fs.file_address + (address + fs.file_size))) 
		return 0;
	
	fs.write(fs.file_address + address, byte);
	
	return 1;
}


uint8_t file_read_byte(uint32_t address, char& byte) {
	
	if (fs.file_address == 0)
		return 0;
	
	address += address / (SECTOR_SIZE - 1); // Add to the address the number of data sectors
	address += SECTOR_SIZE;                 // Skip the first sector / header sector
	address ++;                             // Add one to skip the sector state byte
	
	// Check EOF overflow
	if ((fs.file_address + address) > (fs.file_address + (address + fs.file_size))) 
		return 0;
	
	fs.read(fs.file_address + address, byte);
	
	return 1;
}


void eeprom_wait_state(void) {
	if (console.promptString[0] != '/') 
		_delay_ms(5);
}

uint32_t fs_set_device_scope(void) {
	if (console.promptString[0] == '/') {
		return _VIRTUAL_STORAGE_ADDRESS__;
	} else {
		return (0x40000 + (0x10000 * (console.promptString[0] - 'A')));
	}
}



//
// Associated command line modules

#include "modules/dir.h"      // List files on device
#include "modules/mk.h"       // Make a new file
#include "modules/rm.h"       // Remove a file
#include "modules/rn.h"       // Rename a file
#include "modules/cd.h"       // Change device
#include "modules/copy.h"     // Copy a file to new file
#include "modules/mkfs.h"     // Create a file system on a device
#include "modules/attr.h"     // File attributes



void fileSystemDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	if (functionCall == DEVICE_CALL_INITIATE) {
		
#ifdef __BOOT_FS_SUPPORT_
		
		load_device("dir", sizeof("dir"), (Module)command_dir, DEVICE_TYPE_MODULE);
		load_device("mk", sizeof("mk"), (Module)command_mk, DEVICE_TYPE_MODULE);
		load_device("rm", sizeof("rm"), (Module)command_rm, DEVICE_TYPE_MODULE);
		load_device("rn", sizeof("rn"), (Module)command_rn, DEVICE_TYPE_MODULE);
		load_device("cd", sizeof("cd"), (Module)command_cd, DEVICE_TYPE_MODULE);
		load_device("copy", sizeof("copy"), (Module)command_copy, DEVICE_TYPE_MODULE);
		load_device("attr", sizeof("attr"), (Module)command_attrib, DEVICE_TYPE_MODULE);
		
		load_device("mkfs", sizeof("mkfs"), (Module)command_make_fs, DEVICE_TYPE_MODULE);
		
#endif
		fs.initiate(); return;
	}
	
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
	
	if (functionCall == 0x0d) {paramA = file_open(fs.file_name); return;}
	if (functionCall == 0x0e) {paramA = file_close(); return;}
	if (functionCall == 0x0f) {file_read_byte(fs.device_address, (char&)paramA); return;}
	if (functionCall == 0x10) {file_write_byte(fs.device_address, (char&)paramA); return;}
	
	if (functionCall == 0x11) {fs.read(fs.device_address, (char&)paramA); return;}
	if (functionCall == 0x12) {fs.write(fs.device_address, (char&)paramA); return;}
	
	if (functionCall == 0x2f) {list_directory(paramA); return;}
	
	return;
}


#endif


