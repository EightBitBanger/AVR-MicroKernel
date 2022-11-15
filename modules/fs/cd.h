#ifndef _CD_FUNCTION__
#define _CD_FUNCTION__

void command_cd(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	// Link to the storage driver
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint8_t param0 = console.keyboard_string[sizeof("cd")];
	
	// Lower case
	char string[1];
	string[0] = console.keyboard_string[sizeof("cd")];
	string_lower(string, 1);
	param0 = string[0];
	
	if ((string[0] >= 'a') & (string[0] < ('a' + _HARDWARE_SLOT_COUNT__))) 
		console.promptString[0] = (string[0] - 0x20);
	
	if (string[0] == '/') console.promptString[0] = '/';
	if (string[0] == '-') console.promptString[0] = '-';
	
	return;
}

#endif

