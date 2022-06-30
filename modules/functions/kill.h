//
// Kill command

void command_kill(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "kill"

struct ModuleLoaderKill {
	ModuleLoaderKill() {
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Device)command_kill, _DEVICE_TYPE_MODULE__);
	}
}static loadModuleKill;
#undef __MODULE_NAME_

void command_kill(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device consoleDriver;
	if (get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), consoleDriver) == 0) return;
	
	char keyboard_string[_PROCESS_NAME_LENGTH_MAX__];
	char task_name[_PROCESS_NAME_LENGTH_MAX__];
	for (uint8_t a=0; a < _PROCESS_NAME_LENGTH_MAX__; a++) {keyboard_string[a] = 0x20; task_name[a] = 0x20;}
	
	uint8_t index = console.keyboard_string[5] - 0x30;
	
	if (task_destroy(index) == 0) {
		// Task not stopped
		for(uint8_t i=0; i < sizeof(msg_task_not_found)-1; i++)
			call_extern(consoleDriver, 0x00, (uint8_t&)msg_task_not_found[i]);
	} else {
		// Task stopped
		for(uint8_t i=0; i < sizeof(msg_task_stopped)-1; i++) 
			call_extern(consoleDriver, 0x00, (uint8_t&)msg_task_stopped[i]);
	}
	
	call_extern(consoleDriver, 0x01); // new line
	
	return;
}

