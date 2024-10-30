#include <avr/io.h>

extern "C" {
#include <kernel/kernel.h>
#include <kernel/array.hpp>
}


array::array() : 
    mSize(0),
    mCapacity(0),
    mAddress(0)
{}

array::array(uint32_t size) {
    
    mSize = size;
    mCapacity = size;
    
    mAddress = new(mCapacity);
    
    return;
}

array::array(const array& arr) {
    
    mSize = arr.mSize;
    mCapacity = arr.mCapacity;
    
    mAddress = new(mCapacity);
    
    uint8_t buffer[mSize];
    
    VirtualRead(arr.mAddress, buffer, mSize);
    
    VirtualWrite(mAddress, buffer, mSize);
    
    return;
}

array::~array() {
    
    if (mAddress > 0) 
        delete(mAddress);
    
    return;
}

uint8_t array::operator[](unsigned int const i) {
    
    return get(i);
}

void array::set(unsigned int const i, uint8_t item) {
    
    uint8_t buffer[mSize];
    
    VirtualRead(mAddress, buffer, mSize);
    
    buffer[i] = item;
    
    VirtualWrite(mAddress, buffer, mSize);
    
    return;
}

uint8_t array::get(unsigned int const i) {
    
    uint8_t buffer[mSize];
    
    VirtualRead(mAddress, buffer, mSize);
    
    return buffer[i];
}

uint32_t array::size(void) {
    
    return mSize;
}

uint32_t array::capacity(void) {
    
    return mCapacity;
}

void array::resize(uint32_t newSize) {
    
    reserve(newSize);
    
    mSize = newSize;
    
    return;
}

void array::reserve(uint32_t newCapacity) {
    
    uint8_t buffer[mCapacity];
    
    uint32_t newAddress = new(newCapacity);
    
    VirtualRead(mAddress, buffer, mCapacity);
    
    VirtualWrite(newAddress, buffer, mCapacity);
    
    delete(mAddress);
    
    mAddress = newAddress;
    
    mCapacity = newCapacity;
    
    return;
}

void array::push_back(uint8_t item) {
    
    if (mSize >= mCapacity) {
        if (mCapacity == 0) 
            mCapacity = 1;
        
        reserve( mCapacity * 2 );
    }
    
    mSize++;
    
    set(mSize-1, item);
    
    return;
}
