//
// Device drivers

#include "drivers/display.h"
#include "drivers/keyboard.h"
//#include "drivers/file_system.h"

struct LibraryLoader {
	
	LibraryLoader() {
		
		loadLibrary(_KEYBOARD_INPUT__, sizeof(_KEYBOARD_INPUT__), &keyboardDeviceDriverEntryPoint);
		loadLibrary(_DISPLAY_CONSOLE__, sizeof(_DISPLAY_CONSOLE__), &DisplayDeviceDriverEntryPoint);
		
	}
};
LibraryLoader loadLibraries;


