#include <avr/io.h>

#include <kernel/command/cli.h>

extern uint32_t fs_working_directory_address;

extern uint16_t display_width;
extern uint16_t display_height;

extern uint8_t console_string[CONSOLE_STRING_LENGTH];
extern uint8_t console_string_old[CONSOLE_STRING_LENGTH];
extern uint8_t console_string_length;
extern uint8_t console_string_length_old;

extern uint8_t console_position;
extern uint8_t console_line;

extern uint8_t oldScanCodeLow;
extern uint8_t oldScanCodeHigh;

extern uint8_t lastChar;

extern uint8_t shiftState;

extern uint8_t console_prompt[16];
extern uint8_t console_prompt_length;

extern uint8_t cursor_blink_rate;

extern uint16_t displayRows;
extern uint16_t displayColumbs;

extern struct Driver* displayDevice;
extern struct Driver* keyboadDevice;

extern struct ConsoleCommand CommandRegistry[CONSOLE_FUNCTION_TABLE_SIZE];

uint8_t msgDeviceNotReady[] = "Device not ready";
uint8_t msgBadCommand[] = "Bad cmd or filename";

uint8_t commandSpecialChar(void);
uint8_t commandFunctionLookup(uint8_t params_begin);
uint8_t ExecuteBinDirectory(void);


uint8_t ExecuteFile(uint32_t fileAddress) {
    
    if (fileAddress == 0) 
        return 0;
    
    // Check file is executable
    struct FSAttribute attribute;
    fsFileGetAttributes(fileAddress, &attribute);
    
    // Check executable
    if (attribute.executable != 'x') 
        return 0;
    
    fsFileOpen(fileAddress);
    
    union Pointer fileSizePtr;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(fileAddress + FILE_OFFSET_SIZE + i, &fileSizePtr.byte_t[i]);
    
    uint32_t programSize = fileSizePtr.address;
    
    uint8_t programBuffer[programSize];
    
    // Load the file
    fsFileRead(programBuffer, programSize);
    
    fsFileClose();
    
    // Emulate the code
    EmulateX4(&programBuffer[0], programSize);
    
    return 1;
}


void KeyFunctionReturn(void) {
    
    printLn();
    ConsoleSetCursorPosition(0);
    
    // Save last entered command string
    for (uint8_t a=0; a <= console_string_length; a++) 
        console_string_old[a] = console_string[a];
    
    console_string_length_old = console_string_length;
    
    // Get the filename and parameters
    uint8_t filename_length = 0;
    uint8_t parameters_begin = 0;
    
    for (uint8_t i=0; i <= console_string_length; i++) {
        
        if (console_string[i] != ' ') 
            continue;
        
        filename_length  = i + 1;
        parameters_begin = i + 1;
        
        if (filename_length > FILE_NAME_LENGTH) 
            filename_length = FILE_NAME_LENGTH;
        
        break;
    }
    
    // Check special character functionality
    uint8_t passed = commandSpecialChar();
    
    // Look up function name
    if (passed == 0) {
        
        passed = commandFunctionLookup(parameters_begin);
        
    }
    
    // Check device ready
    if ((passed == 0) & (fsCheckDeviceReady() == 0) & (console_string_length > 0)) {
        
        print(msgDeviceNotReady, sizeof(msgDeviceNotReady));
        printLn();
        
        ConsoleClearKeyboardString();
        console_string_length = 0;
        printPrompt();
        
        return;
    }
    
    //
    // Check file executable
    if ((passed == 0) & (console_string_length > 0)) {
        
        // Check file exists in working directory
        uint32_t fileAddress = fsFileExists(console_string, filename_length);
        
        passed = ExecuteFile( fileAddress );
    }
    
    //
    // Check the bin directory for the executable name
    if (passed == 0) {
        
        passed = ExecuteBinDirectory();
    }
    
    
    //
    // Bad command or filename
    
    if ((passed == 0) & (console_string_length > 0)) {
        
        ConsoleSetCursor(console_line, 0);
        
        print( msgBadCommand, sizeof(msgBadCommand) );
        
        printLn();
        
    }
    
    ConsoleClearKeyboardString();
    
    console_string_length = 0;
    
    printPrompt();
    
    return;
}


uint8_t commandSpecialChar(void) {
    
    //
    // Change device with a colon character
    
    if ((console_string[1] == ':') & 
        (console_string[2] == ' ')) {
        
        uppercase(&console_string[0]);
        
        // Device letter
        if ((console_string[0] < 'A') & 
            (console_string[0] > 'Z')) 
            return 0;
        
        // Update the prompt if the device letter was changed
        
        fsSetDeviceLetter( console_string[0] );
        
        uint8_t consolePrompt[] = {console_string[0], '>'};
        
        ConsoleSetPrompt(consolePrompt, 3);
        
        ConsoleSetCursorPosition(2);
        
        fsClearWorkingDirectory();
        fsWorkingDirectorySetStack(0);
        
        console_string_length = 0;
        
        return 1;
    }
    
    return 0;
}

uint8_t commandFunctionLookup(uint8_t params_begin) {
    
    uint8_t passed = 0;
    uint8_t parameters_begin_chk = 0;
    
    for (uint8_t i=0; i < CONSOLE_FUNCTION_TABLE_SIZE; i++) {
        
        for (uint8_t n=0; n < CONSOLE_FUNCTION_NAME_LENGTH; n++) {
            
            uint8_t consoleChar = console_string[n];
            lowercase(&consoleChar);
            
            if (CommandRegistry[i].name[n] != consoleChar) {
                passed = 0;
                break;
            }
            
            if ((is_symbol(&console_string[n+1]) == 1) | 
                (CommandRegistry[i].name[n] == ' ')) {
                
                if (parameters_begin_chk == 0) 
                    parameters_begin_chk = n + 1;
                
                break;
            }
            
            passed = 1;
        }
        
        if (passed == 0) 
            continue;
        
        if (parameters_begin_chk != 0) 
            params_begin = parameters_begin_chk;
        
        // Run the function
        if (CommandRegistry[i].function != nullptr) 
            CommandRegistry[i].function( &console_string[params_begin], (console_string_length + 1) - params_begin );
        
        break;
    }
    
    return passed;
}


uint8_t ExecuteBinDirectory(void) {
    
    // Get bin directory
    uint32_t pathBinLength = EnvironmentGetPathBinLength();
    
    uint8_t binDirectoryPath[pathBinLength];
    EnvironmentGetPathBin(binDirectoryPath, pathBinLength);
    
    if (pathBinLength == 0) 
        return 0;
    
    // Get current directory
    uint32_t currentWorkingDirectoryAddress = fsWorkingDirectoryGetAddress();
    
    // Set root directory
    fsSetWorkingDirectory( fsFormatGetRootDirectory() );
    
    // Change to home device
    uint8_t deviceLetter = fsGetDeviceRoot();
    lowercase(&deviceLetter);
    
    fsSetDeviceLetter( EnvironmentGetHomeChar() );
    
    uint32_t directoryAddress = fsDirectoryExists(binDirectoryPath, EnvironmentGetPathBinLength()-1);
    
    if (directoryAddress == 0) {
        
        fsSetDeviceLetter( deviceLetter );
        fsSetWorkingDirectory( currentWorkingDirectoryAddress );
        
        return 0;
    }
    
    uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles( directoryAddress );
    
    if (numberOfFiles == 0) {
        
        fsSetDeviceLetter( deviceLetter );
        fsSetWorkingDirectory( currentWorkingDirectoryAddress );
        
        return 0;
    }
    
    for (uint32_t i=0; i < numberOfFiles; i++) {
        
        uint32_t fileAddress = fsDirectoryGetFile(directoryAddress, i);
        
        // Check execute attribute
        struct FSAttribute attribute;
        fsFileGetAttributes(fileAddress, &attribute);
        
        if (attribute.executable != 'x') 
            continue;
        
        uint8_t filename[FILE_NAME_LENGTH];
        fsFileGetName(fileAddress, filename);
        
        // Run the executable
        if (StringCompare(filename, FILE_NAME_LENGTH, console_string, FILE_NAME_LENGTH) == 1) {
            
            fsSetDeviceLetter( deviceLetter );
            fsSetWorkingDirectory( currentWorkingDirectoryAddress );
            
            return ExecuteFile(fileAddress);
        }
        
    }
    
    // Reset previous device and directory
    fsSetDeviceLetter( deviceLetter );
    fsSetWorkingDirectory( currentWorkingDirectoryAddress );
    
    return 1;
}

