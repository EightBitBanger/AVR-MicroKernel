//
// Module plugins

#include "modules/kill.h"
#include "modules/mem.h"
#include "modules/cls.h"
#include "modules/device.h"
#include "modules/task.h"

struct ModuleLoader {
	
	ModuleLoader() {
		
		loadModule(command_memory,       "mem",  4);
		loadModule(command_clear_screen, "cls",  4);
		loadModule(command_device,       "dev",  4);
		loadModule(command_task,         "task", 5);
		loadModule(command_kill,         "kill", 5);
		
	}
};
ModuleLoader loadModules;


