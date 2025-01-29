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





/*



#include <avr/io.h>

extern "C" {
#include <kernel/kernel.h>
}


template<typename T, uint32_t N>
array<T, N>::array() : mSize(0), mCapacity(N), mAddress(0) {
    mAddress = new(mCapacity);
}

template<typename T, uint32_t N>
array<T, N>::array(const array& arr) {
    copy_from(arr);
}

template<typename T, uint32_t N>
array<T, N>& array<T, N>::operator=(const array& arr) {
    if (this != &arr) {
        delete(mAddress);
        copy_from(arr);
    }
    return *this;
}

template<typename T, uint32_t N>
array<T, N>::~array() {
    if (mAddress > 0) 
        delete(mAddress);
}

template<typename T, uint32_t N>
T& array<T, N>::operator[](unsigned int const i) {
    static T dummy;
    VirtualRead(mAddress, mBuffer, sizeof(T));
    if (i < mSize) {
        return reinterpret_cast<T&>(mBuffer[i]);
    }
    // Return a reference to a dummy variable if out of bounds
    return dummy;
}

template<typename T, uint32_t N>
const T& array<T, N>::operator[](unsigned int const i) const {
    static T dummy;
    VirtualRead(mAddress, mBuffer, sizeof(T));
    if (i < mSize) {
        return reinterpret_cast<const T&>(mBuffer[i]);
    }
    // Return a reference to a dummy variable if out of bounds
    return dummy;
}

template<typename T, uint32_t N>
void array<T, N>::set(unsigned int const i, T item) {
    uint8_t buffer[sizeof(T)];
    buffer[i] = reinterpret_cast<uint8_t&>(item);
    VirtualWrite(mAddress, buffer, sizeof(T));
}

template<typename T, uint32_t N>
T array<T, N>::get(unsigned int const i) const {
    uint8_t buffer[sizeof(T)];
    VirtualRead(mAddress, buffer, sizeof(T));
    return reinterpret_cast<T&>(buffer[i]);
}

template<typename T, uint32_t N>
uint32_t array<T, N>::size() const {
    return mSize;
}

template<typename T, uint32_t N>
uint32_t array<T, N>::capacity() const {
    return mCapacity;
}

template<typename T, uint32_t N>
void array<T, N>::resize(uint32_t newSize) {
    reserve(newSize);
    mSize = newSize;
}

template<typename T, uint32_t N>
void array<T, N>::reserve(uint32_t newCapacity) {
    if (newCapacity <= mCapacity) return;
    T buffer[mCapacity];
    uint32_t newAddress = new(newCapacity);
    VirtualRead(mAddress, buffer, mCapacity);
    VirtualWrite(newAddress, buffer, mCapacity);
    delete(mAddress);
    mAddress = newAddress;
    mCapacity = newCapacity;
}

template<typename T, uint32_t N>
void array<T, N>::push_back(T item) {
    if (mSize >= mCapacity) {
        if (mCapacity == 0)
            mCapacity = 1;
        reserve(mCapacity * 2);
    }
    mSize++;
    set(mSize-1, item);
}

template<typename T, uint32_t N>
void array<T, N>::copy_from(const array& arr) {
    mSize = arr.mSize;
    mCapacity = arr.mCapacity;
    mAddress = new(mCapacity);
    T buffer[mSize];
    VirtualRead(arr.mAddress, buffer, mSize);
    VirtualWrite(mAddress, buffer, mSize);
}



*/



