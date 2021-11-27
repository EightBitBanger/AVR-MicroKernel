//
// Launch system service routine functions



void launchServiceRoutines(void) {
	
	// Launch the keyboard handler service
	const char keyboardService[] = "kbsrv";
	scheduler.createTask(keyboardService, sizeof(keyboardService), keyboard_event_handler, 1000, _TASK_TYPE_SERVICE__);
	
	const char testService[] = "testsrv";
	scheduler.createTask(testService, sizeof(testService), NULL_f, 10000, _TASK_TYPE_USER__);
	
	
	
	
}

