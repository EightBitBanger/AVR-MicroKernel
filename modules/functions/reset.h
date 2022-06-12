//
// Soft reset

void __reset_main_(void);

struct ModuleLoaderReset {
	ModuleLoaderReset() {
		load_module("reset", sizeof("reset"), __reset_main_);
	}
}static moduleLoaderReset;

//
// Entry point
int main(void);

void __reset_main_(void) {
	
	void(*functionPtr)();
	functionPtr = (void(*)()) main;
	
	functionPtr();
	
	return;
}


