//
// Application command function table

#ifndef ____COMMAND_MODULE_TABLE__
#define ____COMMAND_MODULE_TABLE__

#define _COMMAND_TABLE_SIZE__        20  // Total number of function modules
#define _COMMAND_NAME_LENGTH_MAX__    8  // Max name length

#ifdef  __CORE_MAIN_


// Install a module onto the function table
uint8_t load_module(const char name[], uint8_t name_length, void(*function_ptr)());

void __module_init_(void);



struct CommandFunctionTable {
	
	char functionNameIndex[_COMMAND_TABLE_SIZE__][_COMMAND_NAME_LENGTH_MAX__];
	void (*command_function_table[_COMMAND_TABLE_SIZE__])(void);
	
}volatile static moduleTable;


uint8_t load_module(const char name[], uint8_t name_length, void(*function)()) {
	
	if (name_length > _COMMAND_NAME_LENGTH_MAX__) return 0;
	
	uint8_t index;
	for (index=0; index < _COMMAND_TABLE_SIZE__; index++)
		if (moduleTable.functionNameIndex[index][0] == 0x20) break;
	
	for (uint8_t i=0; i < name_length-1; i++)
		moduleTable.functionNameIndex[index][i] = name[i];
	
	moduleTable.command_function_table[index] = function;
	
	return 1;
}

void __module_init_(void) {
	for (uint8_t i=0; i < _COMMAND_TABLE_SIZE__; i++) {
		for (uint8_t a=0; a < _COMMAND_NAME_LENGTH_MAX__; a++)
			moduleTable.functionNameIndex[i][a] = 0x20;
	}
}

#endif

#endif


