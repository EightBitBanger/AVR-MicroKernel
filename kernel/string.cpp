#include <avr/io.h>

extern "C" {
#include <kernel/kernel.h>
#include <kernel/string.h>
}

string::string() : 
    mAddress(0),
    mLength(0)
{
}

string::string(const string& str) {
    
    mLength = str.mLength;
    
    mAddress = new(mLength);
    
    uint8_t buffer[mLength];
    
    VirtualRead(str.mAddress, buffer, mLength+1);
    
    VirtualWrite(mAddress, buffer, mLength+1);
    
    return;
}

string::~string() {
    
    if (mAddress > 0) 
        delete(mAddress);
    
    return;
}

uint32_t string::size(void) {
    
    return mLength;
}

void string::resize(uint32_t newSize) {
    
    if (mAddress == 0) 
        return;
    
    uint32_t newAddress = 0;
    
    uint8_t buffer[newSize];
    
    newAddress = new(newSize);
    
    if (newSize > mLength) {
        
        VirtualRead(mAddress, buffer, mLength);
        
        VirtualWrite(newAddress, buffer, mLength);
        
    } else {
        
        VirtualRead(mAddress, buffer, newSize);
        
        VirtualWrite(newAddress, buffer, newSize);
    }
    
    delete(mAddress);
    
    mAddress = newAddress;
    mLength = newSize;
    
    return;
}

void string::data(uint8_t* buffer, uint32_t length) {
    
    VirtualRead(mAddress, buffer, length);
    
    return;
}


void string::operator= (uint8_t* source) {
    
    if (mAddress > 0) 
        delete(mAddress);
    
    mLength = 0;
    
    for (uint32_t i=0; i < 1024; i++) {
        
        mLength++;
        
        if (source[i] == '\0') 
            break;
        
    }
    
    mAddress = new(mLength);
    
    VirtualWrite(mAddress, source, mLength+1);
    
    return;
}

uint8_t string::operator[] (unsigned int const i) {
    
    if (i > mLength) 
        return 0;
    
    uint8_t buffer[mLength];
    
    VirtualRead(mAddress, buffer, mLength);
    
    return buffer[i];
}
