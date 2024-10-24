#ifndef __ARRAY_ALLOCATOR_
#define __ARRAY_ALLOCATOR_


class array {
    
public:
    
    array() : 
        mSize(0),
        mCapacity(0),
        mAddress(0)
    {}
    
    array(const array& arr) {
        
        mSize = arr.mSize;
        mCapacity = arr.mCapacity;
        
        mAddress = new(mCapacity);
        
        uint8_t buffer[mSize];
        
        VirtualRead(arr.mAddress, buffer, mSize);
        
        VirtualWrite(mAddress, buffer, mSize);
        
        return;
    }
    
    ~array() {
        
        if (mAddress > 0) 
            delete(mAddress);
        
        return;
    }
    
    uint8_t operator[](unsigned int const i) {
        
        uint8_t buffer[mSize];
        
        if (i >= mSize) 
            return 0;
        
        VirtualRead(mAddress, buffer, mSize);
        
        return buffer[i];
    }
    
    void set(unsigned int const i, uint8_t item) {
        
        if (i >= mSize) 
            return;
        
        uint8_t buffer[mSize];
        
        VirtualRead(mAddress, buffer, mSize);
        
        // Write to the buffer
        buffer[i] = item;
        
        VirtualWrite(mAddress, buffer, mSize);
        
        return;
    }
    
    uint32_t size(void) {return mSize;}
    
    uint32_t capacity(void) {return mCapacity;}
    
    void resize(uint32_t newSize) {
        
        reserve(newSize);
        
        mSize = newSize;
        
        return;
    }
    
    void reserve(uint32_t newCapacity) {
        
        uint8_t buffer[mCapacity];
        
        uint32_t newAddress = new(newCapacity);
        
        VirtualRead(mAddress, buffer, mCapacity);
        
        VirtualWrite(newAddress, buffer, mCapacity);
        
        delete(mAddress);
        
        mAddress = newAddress;
        
        mCapacity = newCapacity;
        
        return;
    }
    
    void push_back(uint8_t item) {
        
        if (mSize >= mCapacity) {
            if (mCapacity == 0) 
                mCapacity = 1;
            
            reserve( mCapacity * 2 );
        }
        
        set(mSize, item);
        
        mSize++;
        
        return;
    }
    
private:
    
    uint32_t mSize;
    uint32_t mCapacity;
    uint32_t mAddress;
    
};




/*
template <typename T> class Array {
    
public:
    
    Array(size_t initialCapacity) : 
        mSize(0),
        mCapacity(initialCapacity) 
    {
        mData = new T[mCapacity];
    }
    
    ~Array() {
        delete[] mData;
    }
    
    void add(const T& element) {
        
        // Double the capacity
        if (mSize >= mCapacity) {
            resize(mCapacity * 2);
        }
        
        mData[mSize++] = element;
    }
    
    T* get(size_t index) {
        
        if (index < mSize) {
            return &mData[index];
        }
        
        return nullptr;
    }
    
    bool remove(size_t index) {
        
        if (index >= mSize) {
            return false;
        }
        
        for (size_t i = index; i < mSize - 1; i++) {
            mData[i] = mData[i + 1];
        }
        
        mSize--;
        
        return true;
    }
    
    size_t getSize() const {
        return mSize;
    }
    
    size_t getCapacity() const {
        return mCapacity;
    }
    
private:
    T* mData;       // Pointer to the array of type T
    size_t mSize;   // Current size of the array
    size_t mCapacity; // Maximum capacity of the array
    
    void resize(size_t newCapacity) {
        T* newData = new T[newCapacity];
        for (size_t i = 0; i < mSize; i++) {
            newData[i] = mData[i];
        }
        delete[] mData;
        mData = newData;
        mCapacity = newCapacity;
    }
    
};



*/


#endif
