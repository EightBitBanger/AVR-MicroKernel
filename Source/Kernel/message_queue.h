
// System message queue
struct MessageQueue {
	
	uint8_t queue[8];
	uint8_t counter;
	
	// Message callback
	uint8_t (*callbackPointer)(uint8_t);
	
	MessageQueue() {
		
		// Initiate message queue
		for (uint8_t i=0; i < 8; i++) queue[i] = 0xff;
		counter=0;
		
		// Set default message callback procedure
		callbackPointer = &defaultCallbackProcedure;
		
	}
	
	// Post a message to the kernel
	void postMessage(uint8_t message) {
		
		// Shift down the queue
		queue[7] = queue[6];
		queue[6] = queue[5];
		queue[5] = queue[4];
		queue[4] = queue[3];
		queue[3] = queue[2];
		queue[2] = queue[1];
		queue[1] = queue[0];
		
		// Add to queue
		queue[0] = message;
		counter++;
		
		return;
	}
	// Check the number of messages currently in the queue
	uint8_t peekMessage(void) {return counter;}
	// Process a message from the message queue
	void processMessage(void) {
		
		if (counter == 0) return;
		
		// Grab the first message
		uint8_t message = queue[0];
		
		// Shift up the queue
		queue[0] = queue[1];
		queue[1] = queue[2];
		queue[2] = queue[3];
		queue[3] = queue[4];
		queue[4] = queue[5];
		queue[5] = queue[6];
		queue[6] = queue[7];
		
		// Decrement the queue
		counter--;
		
		// Call the callback procedure function with the message
		(callbackPointer)( message );
		
	}
	
};
MessageQueue queue;

