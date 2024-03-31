#include <avr/io.h>
#include <util/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/net.h>

void functionNet(uint8_t* param, uint8_t param_length) {
    
    lowercase( &param[0] );
    lowercase( &param[1] );
    lowercase( &param[2] );
    lowercase( &param[3] );
    
    //
    // Set the baud rate
    //
    
    if ((param[0] == 'b') & (param[1] == 'a') & (param[2] == 'u') & (param[3] == 'd')) {
        
        uint8_t msg[] = "Baud set = ?   ";
        
        if (param[5] == '0') {msg[11] = '9'; msg[12] = '0'; msg[13] = '0';                ntSetBaudRate(0);}
        if (param[5] == '1') {msg[11] = '1'; msg[12] = '2'; msg[13] = '0'; msg[14] = '0'; ntSetBaudRate(1);}
        if (param[5] == '2') {msg[11] = '2'; msg[12] = '4'; msg[13] = '0'; msg[14] = '0'; ntSetBaudRate(2);}
        if (param[5] == '3') {msg[11] = '4'; msg[12] = '8'; msg[13] = '0'; msg[14] = '0'; ntSetBaudRate(3);}
        if (param[5] == '4') {msg[11] = '9'; msg[12] = '6'; msg[13] = '0'; msg[14] = '0'; ntSetBaudRate(4);}
        if (param[5] == '5') {msg[11] = '2'; msg[12] = '8'; msg[13] = 'K';                ntSetBaudRate(5);}
        if (param[5] == '6') {msg[11] = '3'; msg[12] = '3'; msg[13] = 'K';                ntSetBaudRate(6);}
        if (param[5] == '7') {msg[11] = '5'; msg[12] = '6'; msg[13] = 'K';                ntSetBaudRate(7);}
        if (param[5] == '8') {msg[11] = '7'; msg[12] = '6'; msg[13] = 'K';                ntSetBaudRate(8);}
        if (param[5] == '9') {msg[11] = '1'; msg[12] = '2'; msg[13] = '5'; msg[14] = 'K'; ntSetBaudRate(9);}
        
        print(msg, sizeof(msg));
        printLn();
        
        return;
    }
    
    //
    // Send the parameter message
    //
    
    if ((param[0] == 's') & (param[1] == 'e') & (param[2] == 'n') & (param[3] == 'd')) {
        
        uint8_t offset = 10;
        
        struct NetworkPacket packet;
        
        for (uint8_t i=0; i < 16; i++) 
            packet.data[i] = 0x55;
        
        packet.start   = NETWORK_PACKET_START_BYTE;
        
        packet.addr_d[0]   = 0x00;
        packet.addr_d[1]   = 0x00;
        packet.addr_s[0]   = 0x01;
        packet.addr_s[1]   = 0x00;
        
        // Indicate we are a client attempting to connect to a server
        packet.data[0] = 0x55;
        packet.data[1] = 0x00;
        
        for (uint8_t i=0; i < (param_length - offset) + 1; i++) 
            packet.data[i] = param[offset + i];
        
        packet.stop    = NETWORK_PACKET_STOP_BYTE;
        
        // Start byte to indicate we are starting to send a message to the server
        ntSend(&packet.start, 1);
        
        // Send the destination and source address
        // NOTE: The source address is the address referring 
        // to the client device who is making the request.
        ntSend(&packet.addr_d[0], 2);
        ntSend(&packet.addr_s[0], 2);
        
        ntSend(&packet.data[0], 16);
        
        ntSend(&packet.stop, 1);
        
        return;
    }
    
    //
    // Connect to a server, adding your address to the routing table
    //
    
    if ((param[0] == 'c') & (param[1] == 'o') & (param[2] == 'n')) {
        
        struct NetworkPacket packet;
        
        for (uint8_t i=0; i < 16; i++) 
            packet.data[i] = 0x55;
        
        packet.start   = NETWORK_PACKET_START_BYTE;
        
        packet.addr_d[0]   = 0xff;
        packet.addr_d[1]   = 0xff;
        packet.addr_s[0]   = 0x01;
        packet.addr_s[1]   = 0x00;
        
        // Indicate we are a client attempting to connect to a server
        packet.data[0] = 0x55;
        packet.data[1] = 0x00;
        
        packet.stop    = NETWORK_PACKET_STOP_BYTE;
        
        // Start byte to indicate we are starting to send a message to the server
        ntSend(&packet.start, 1);
        
        // Send the destination and source address
        // NOTE: The source address is the address referring 
        // to the client device who is making the request.
        ntSend(&packet.addr_d[0], 2);
        ntSend(&packet.addr_s[0], 2);
        
        ntSend(&packet.data[0], 16);
        
        ntSend(&packet.stop, 1);
        
        return;
    }
    
    //
    // Ping the server
    //
    
    if ((param[0] == 'p') & (param[1] == 'i') & (param[2] == 'n') & (param[3] == 'g')) {
        
        uint8_t offset = 10;
        
        struct NetworkPacket packet;
        
        for (uint8_t i=0; i < 16; i++) 
            packet.data[i] = 0x55;
        
        packet.start   = NETWORK_PACKET_START_BYTE;
        
        packet.addr_d[0]   = 0xff;
        packet.addr_d[1]   = 0xff;
        packet.addr_s[0]   = 0x00;
        packet.addr_s[1]   = 0x00;
        
        // Indicate we are a client attempting to connect to a server
        packet.data[0] = 0x55;
        packet.data[1] = 0x00;
        
        for (uint8_t i=0; i < (param_length - offset) + 1; i++) 
            packet.data[i] = param[offset + i];
        
        packet.stop    = NETWORK_PACKET_STOP_BYTE;
        
        ntReceiveClear();
        
        for (uint8_t i=0; i < 1; i++) {
            
            // Start byte to indicate we are starting to send a message to the server
            ntSend(&packet.start, 1);
            
            // Send the destination and source address
            // NOTE: The source address is the address referring 
            // to the client device who is making the request.
            ntSend(&packet.addr_d[0], 2);
            ntSend(&packet.addr_s[0], 2);
            
            // Send the packet data
            ntSend(&packet.data[0], 16);
            
            ntSend(&packet.stop, 1);
            
            uint8_t dataBuffer[32];
            
            uint8_t bufferSz = 0;
            
            for (uint16_t i=0; i < 10000; i++) {
                
                bufferSz = ntReceive(dataBuffer, 32);
                
                ntReceiveClear();
                
                if (bufferSz > 0) {
                    uint8_t addrStrLow[8];
                    uint8_t addrStrHigh[8];
                    
                    int_to_string(packet.addr_d[0], addrStrLow);
                    int_to_string(packet.addr_d[1], addrStrHigh);
                    
                    uint8_t msgReplyFrom[] = "Reply from ";
                    print(msgReplyFrom, sizeof(msgReplyFrom));
                    
                    print(addrStrLow, 4);
                    
                    uint8_t msgPeriod[] = ".";
                    print(msgPeriod, sizeof(msgPeriod));
                    
                    print(addrStrHigh, 4);
                   
                    printLn();
                    
                    break;
                }
                
                _delay_us(1);
                
            }
            
            if (bufferSz > 0) 
                continue;
            
            // Connection timed out
            
            _delay_ms(1000);
            
            uint8_t msgRequestTimedOut[] = "Request timed out";
            print(msgRequestTimedOut, sizeof(msgRequestTimedOut));
            printLn();
            
            continue;
        }
        
        return;
    }
    
    return;
}


void registerCommandNet(void) {
    
    uint8_t netCommandName[] = "net";
    
    ConsoleRegisterCommand(netCommandName, sizeof(netCommandName), functionNet);
    
    return;
}
