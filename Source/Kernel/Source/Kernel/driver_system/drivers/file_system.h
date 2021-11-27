//
// File allocation system

#define _CHUNK_SIZE__             33
#define _FILE_NAME_MAX_LENGTH__   8

// File header byte offsets
#define _FILESIZE_OFFSET__        1
#define _FILE_ATTRIBUTE_OFFSET__  10
#define _FILENAME_OFFSET__        11

#define _FILE_ATTRIBUTE_NORMAL__  0
#define _FILE_ATTRIBUTE_HIDDEN__  1
#define _FILE_ATTRIBUTE_SYSTEM__  2


struct Partition {
	
	// Partition starting location in extended memory
	uint32_t address_pointer;
	// Size of the allocation
	uint32_t size;
	
	char& operator[] (uint32_t address) {return kernel[address_pointer + address];}
	
	Partition() {
		address_pointer = 0;
		size = 0;
	}
	
	~Partition() {
		if (size > 0) stack_free(size);
	}
	
	// Create a partition
	void create(uint32_t size_bytes) {
		address_pointer = stack_alloc(size_bytes);
		size = size_bytes;
	}
	
	// Deletes the current partition
	void destroy(void) {
		if (size > 0) stack_free(size);
	}
	
	//
	// Format the partition (RAM drive only)
	void format(uint32_t size_bytes) {
		
		// Dump the cache
		kernel[0x00000];
		
		// Mark chunks empty
		for (uint32_t i=0; i<size_bytes; i+=_CHUNK_SIZE__) 
			memory_write(address_pointer + i, 0x00);
		
		
		//
		// Initiate the partition descriptor table
		
		// Reserve the first couple chunks for the partition table
		for (uint8_t i=0; i < (_CHUNK_SIZE__ * 2); i+=_CHUNK_SIZE__) 
			memory_write(address_pointer + i, 0xff);
		
		// Partition size
		setSizeBytes(size_bytes);
		
		// Number of files
		setNumberOfFiles(0);
		
	}
	
	// Returns the number of bytes
	uint32_t getSizeBytes(void) {
		WrappedPointer partitionSz;
		for (uint8_t i=1; i<5; i++) {memory_read(address_pointer + i, partitionSz.byte[i-1]);}
		return partitionSz.address;
	}
	
	// Returns the number of files in the partition
	uint32_t getNumberOfFiles(void) {
		WrappedPointer numberOfFiles;
		for (uint8_t i=5; i<9; i++) memory_read(address_pointer + i, numberOfFiles.byte[i-5]);
		return numberOfFiles.address;
	}
	
	// Sets the size of the partition in bytes
	void setSizeBytes(uint32_t byteSz) {
		WrappedPointer partitionSz(byteSz);
		for (uint8_t i=1; i<5; i++) {memory_write(address_pointer + i, partitionSz.byte[i-1]);}
		return;
	}
	
	// Sets the number of files in the partition
	void setNumberOfFiles(uint32_t numberOfFiles) {
		WrappedPointer fileCounter(numberOfFiles);
		for (uint8_t i=5; i<9; i++) memory_write(address_pointer + i, fileCounter.byte[i-5]);
		return;
	}
	
};
static Partition partition;

struct FileSystem {
	
	// Pointer to open file
	uint32_t current_file_ptr;
	uint32_t current_file_size;
	uint32_t seek;
	
	// File find counter
	uint32_t current_file_find;
	
	FileSystem() {
		
		current_file_ptr=0;
		current_file_size=0;
		seek=0;
		
		current_file_find=0;
		
	}
	
	// Start looking at the first file on the partition
	uint8_t file_find_first(void) {
		
		uint32_t chunk_offset=0;
		uint32_t chunk_counter=0;
		
		// Get partition size
		uint32_t partitionSz = partition.getSizeBytes();
		
		// Check partition chunks
		for (chunk_offset=0; chunk_offset <= partitionSz; chunk_offset += _CHUNK_SIZE__) {
			
			// Continue if the chunk is free
			if (partition[chunk_offset] != 0xff) {continue;}
			
			// Get size
			WrappedPointer file_size_bytes;
			for (uint8_t i=0; i<4; i++) {file_size_bytes.byte[i] = partition[ chunk_offset + i + _FILESIZE_OFFSET__ ];}
			
			// Get attribute
			uint8_t attribute = partition[ chunk_offset + _FILE_ATTRIBUTE_OFFSET__ ];
			
			// Get name
			char filename[8];
			for (uint8_t i=0; i<8; i++) {filename[i] = partition[ chunk_offset + i + _FILENAME_OFFSET__ ];}
			
			current_file_find = chunk_offset;
			
			return 1;
			
		}
		
		return 0;
	}
	
	// Continue looking at the next file(s) on the partition
	void file_find_next(void) {
		
	}
	
	
	// Create a new file on the partition
	uint8_t file_create(Partition& partition, uint32_t size_bytes, const char file_name[], uint8_t name_length, uint8_t attribute=0x00) {
		
		uint32_t chunk_offset=0;
		uint32_t chunk_counter=0;
		
		// Get partition size
		uint32_t partitionSz = partition.getSizeBytes();
		
		// Add extra chunk for file header
		size_bytes += _CHUNK_SIZE__;
		
		// Prepare filename
		char filename[_FILE_NAME_MAX_LENGTH__];
		for (uint8_t i=0; i<_FILE_NAME_MAX_LENGTH__; i++) filename[i] = 0x20;
		for (uint8_t i=0; i<name_length; i++) filename[i] = file_name[i];
		
		// Check partition chunks
		for (chunk_offset=0; chunk_offset < partitionSz; chunk_offset += _CHUNK_SIZE__) {
			
			// Continue if chunk is taken
			if (partition[chunk_offset] == 0xff) {continue;}
			
			// Count free chunks
			for (chunk_counter = chunk_offset; chunk_counter <= size_bytes; chunk_counter+=_CHUNK_SIZE__) {
				
				if (partition[chunk_counter] == 0xff) {chunk_offset = chunk_counter; continue;}
				
			}
			
			// Create file
			// Mark chunks as reserved
			for (uint32_t i=chunk_offset; i < chunk_counter; i+=_CHUNK_SIZE__) {partition[i] = 0xff;}
			
			// Set size
			WrappedPointer file_size_bytes(size_bytes);
			for (uint8_t i=0; i<4; i++) {
				partition[ chunk_offset + i + _FILESIZE_OFFSET__ ] = file_size_bytes.byte[i];
			}
			
			// Set attribute
			partition[ chunk_offset + _FILE_ATTRIBUTE_OFFSET__ ] = attribute;
			
			// Set filename
			for (uint8_t i=0; i < _FILE_NAME_MAX_LENGTH__; i++) {
				partition[ chunk_offset + i + _FILENAME_OFFSET__ ] = filename[i];
			}
			
			// Increment partition file counter
			partition.setNumberOfFiles(partition.getNumberOfFiles() + 1);
			
			partition[0]; //Flush the cache
			
			return 1;
		}
		
		return 0;
	}
	
	// Delete a file from the partition
	uint8_t file_delete(Partition& partition, const char file_name[], uint8_t name_length) {
		
		uint32_t chunk_offset=0;
		
		// Get partition size
		uint32_t partitionSz = partition.getSizeBytes();
		
		// Check partition chunks
		for (chunk_offset=0; chunk_offset <= partitionSz; chunk_offset += _CHUNK_SIZE__) {
			
			// Continue if the chunk is free
			if (partition[chunk_offset] != 0xff) {continue;}
			
			// Get size
			WrappedPointer file_size_bytes;
			for (uint8_t i=0; i<4; i++) {file_size_bytes.byte[i] = partition[ chunk_offset + i + _FILESIZE_OFFSET__ ];}
			
			// Get attribute
			uint8_t attribute = partition[ chunk_offset + _FILE_ATTRIBUTE_OFFSET__ ];
			
			// Get name
			char filename[8];
			for (uint8_t i=0; i<name_length; i++) {filename[i] = partition[ chunk_offset + i + _FILENAME_OFFSET__ ];}
			
			// Check filename match
			uint8_t pass=1;
			for (uint32_t i=0; i < name_length; i++) {if (file_name[i] != filename[i]) {pass = 0; break;}}
			if (pass == 0) continue;
			
			// Free deleted chunks
			for (uint32_t i=chunk_offset; i < file_size_bytes.address; i += _CHUNK_SIZE__) {
				partition[i] = 0x00;
			}
			
			// Decrement partition file counter
			partition.setNumberOfFiles(partition.getNumberOfFiles() - 1);
			
			partition[0]; //Flush the cache
			
			return 1;
			
		}
		
		return 0;
	}
	
	
	
	uint8_t file_open(Partition& partition, const char file_name[], uint8_t name_length) {
		
		uint32_t chunk_offset=0;
		uint32_t chunk_counter=0;
		
		// Check file is open
		if (current_file_ptr > 0) return 0;
		
		// Get partition size
		uint32_t partitionSz = partition.getSizeBytes();
		
		// Check partition chunks
		for (chunk_offset=0; chunk_offset <= partitionSz; chunk_offset += _CHUNK_SIZE__) {
			
			// Continue if the chunk is free
			if (partition[chunk_offset] != 0xff) {continue;}
			
			// Get size
			WrappedPointer file_size_bytes;
			for (uint8_t i=0; i<4; i++) {file_size_bytes.byte[i] = partition[ chunk_offset + i + _FILESIZE_OFFSET__ ];}
			
			// Get attribute
			uint8_t attribute = partition[ chunk_offset + _FILE_ATTRIBUTE_OFFSET__ ];
			
			// Get name
			char filename[8];
			for (uint8_t i=0; i<name_length; i++) {filename[i] = partition[ chunk_offset + i + _FILENAME_OFFSET__ ];}
			
			// Check filename match
			uint8_t pass=1;
			for (uint32_t i=0; i < name_length; i++) {if (file_name[i] != filename[i]) {pass = 0; break;}}
			if (pass == 0) continue;
			
			// Open file
			current_file_ptr  = chunk_offset + _CHUNK_SIZE__;
			current_file_size = file_size_bytes.address;
			
			partition[0]; //Flush the cache
			return 1;
			
		}
		
		return 0;
	}
	
	// Close a file
	uint8_t file_close(Partition& partition) {
		if (current_file_ptr != 0) {
			
			current_file_ptr  = 0;
			current_file_size = 0;
			
			partition[0]; //Flush the cache
			return 1;
		}
		return 0;
	}
	
	// Write a byte to a file
	uint8_t file_write(Partition& partition, char data_byte) {
		
		// Check file size
		if (seek > current_file_size) return 0;
		
		// Set pointer to the current file
		uint32_t offset = (current_file_ptr + seek);
		// Add extra byte per chunk to bypass the chunk state flag
		offset += (seek / _CHUNK_SIZE__);
		
		partition[offset] = data_byte;
		seek++;
		
		return 1;
	}
	
	uint8_t file_read(Partition& partition, char& data_buffer) {
		
		// Check file size
		if (seek > current_file_size) return 0;
		
		// Set pointer to the current file
		uint32_t offset = (current_file_ptr + seek);
		// Add extra byte per chunk to bypass the chunk state flag
		offset += (seek / _CHUNK_SIZE__);
		
		data_buffer = partition[offset];
		seek++;
		
		return 1;
	}
	
	
	
};
static FileSystem system;

void fileSystemLibraryEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD);

struct FileSystemLibrary {
	
	void initiate(void) {
		
	}
	
	void shutdown(void) {
		
	}
	
	FileSystemLibrary() {
		
		const char libraryName[] = "filesys";
		loadLibrary("filesys", 8, &fileSystemLibraryEntryPoint);
		
	}
	
	
	
};
FileSystemLibrary fileSystemLibrary;

void fileSystemLibraryEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	switch(functionCall) {
		
		case _DRIVER_INITIATE__: {fileSystemLibrary.initiate(); break;}
		case _DRIVER_SHUTDOWN__: {fileSystemLibrary.shutdown(); break;}
		
		case 0x00: {partition.create(paramA * 1024); break;}
		
		default: break;
	}
	
	return;
}

