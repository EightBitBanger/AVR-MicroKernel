//
// Launch system service routine functions

void taskTestng(void) {
	
	
	
	return;
}

void launchServiceRoutines(void) {
	
	// Launch the keyboard handler service
	const char keyboardService[] = "kbsrv";
	scheduler.createTask(keyboardService, sizeof(keyboardService), keyboard_event_handler, 1000, _TASK_TYPE_SERVICE__);
	
	// TESTING
	const char ServiceTest[] = "filesystem";
	scheduler.createTask(ServiceTest, sizeof(ServiceTest), taskTestng, 10000, _TASK_TYPE_USER__);
	
	const char UserTest[] = "srvhost";
	scheduler.createTask(UserTest, sizeof(UserTest), taskTestng, 10000, _TASK_TYPE_USER__);
	
	
}

