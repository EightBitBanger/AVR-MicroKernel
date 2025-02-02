#include <avr/io.h>

extern "C" {
#include <kernel/kernel.h>
#include <kernel/string.hpp>
}

string::string() : mSize(0), mCapacity(0), mAddress(0), mDummyIndex(-1), mDummy(0) {}

string::string(uint32_t size) : mSize(size), mCapacity(size), mAddress(new(size)), mDummyIndex(-1), mDummy(0) {}

string::string(const string& arr) : mSize(arr.mSize), mCapacity(arr.mCapacity), mAddress(new(arr.mCapacity)), mDummyIndex(-1), mDummy(0) {
    uint8_t buffer[mSize];
    VirtualRead(arr.mAddress, buffer, mSize);
    VirtualWrite(mAddress, buffer, mSize);
}

string::~string() {
    if (mAddress > 0) 
        delete(mAddress);
}

uint8_t& string::operator[](unsigned int const i) {
    if (mDummyIndex != i) {
        updateBuffer(mDummyIndex, mDummy);
    }
    mDummyIndex = i;
    mDummy = readBuffer(i);
    return mDummy;
}

const uint8_t& string::operator[](unsigned int const i) const {
    mDummy = readBuffer(i);
    return mDummy;
}

void string::set(uint32_t i, uint8_t item) {
    if (i < mSize) {
        updateBuffer(i, item);
    }
}

uint8_t string::get(uint32_t i) {
    if (i < mSize) {
        return readBuffer(i);
    }
    return 0;
}

bool string::empty(void) {
    return mSize == 0;
}

uint32_t string::size(void) {
    return mSize;
}

uint32_t string::max_size(void) {
    return mCapacity;
}

void string::resize(uint32_t newSize) {
    reserve(newSize);
    mSize = newSize;
}

void string::reserve(uint32_t newCapacity) {
    uint8_t buffer[mCapacity];
    uint32_t newAddress = new(newCapacity);
    VirtualRead(mAddress, buffer, mCapacity);
    VirtualWrite(newAddress, buffer, mCapacity);
    delete(mAddress);
    mAddress = newAddress;
    mCapacity = newCapacity;
}

void string::updateBuffer(uint32_t index, uint8_t value) {
    uint8_t buffer[mSize];
    VirtualRead(mAddress, buffer, mSize);
    buffer[index] = value;
    VirtualWrite(mAddress, buffer, mSize);
}

uint8_t string::readBuffer(uint32_t index) const {
    uint8_t buffer[mSize];
    VirtualRead(mAddress, buffer, mSize);
    return buffer[index];
}
