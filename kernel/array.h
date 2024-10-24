#ifndef __ARRAY_ALLOCATOR_
#define __ARRAY_ALLOCATOR_


template<typename T> class array {
    
public:
    
    array() : 
        mSize(0),
        mCapacity(0),
        mAddress(0)
    {}
    
    array(const array& arr) {
        
        mSize = arr.mSize;
        mCapacity = arr.mCapacity;
        
        mAddress = new(mCapacity * sizeof(T));
        
        uint8_t buffer[mSize * sizeof(T)];
        
        VirtualRead(arr.mAddress, buffer, mSize * sizeof(T));
        
        VirtualWrite(mAddress, buffer, mSize * sizeof(T));
        
        return;
    }
    
    ~array() {
        
        if (mAddress > 0) 
            delete(mAddress);
        
        return;
    }
    
    T operator[](unsigned int const i) {
        
        uint8_t buffer[mSize * sizeof(T)];
        
        if (i >= mSize) 
            return (T)buffer[mSize * sizeof(T)];
        
        VirtualRead(mAddress, buffer, mSize * sizeof(T));
        
        return buffer[i * sizeof(T)];
    }
    
    void set(unsigned int const i, T ptr) {
        
        if (i >= mSize) 
            return;
        
        uint8_t buffer[mSize * sizeof(T)];
        
        VirtualRead(mAddress, buffer, mSize * sizeof(T));
        
        // Write to the buffer
        T* beginRef = (T*)buffer[i * sizeof(T)];
        *beginRef = ptr;
        
        VirtualWrite(mAddress, buffer, mSize * sizeof(T));
        
        return;
    }
    
    uint32_t size(void) {return mSize;}
    
    void resize(uint32_t newSize) {
        
        if (newSize > mCapacity) {
            
            uint8_t buffer[newSize * sizeof(T)];
            
            uint32_t newAddress = new(newSize * sizeof(T));
            
            if (newSize > mSize) {
                
                VirtualRead(mAddress, buffer, mSize * sizeof(T));
                
                VirtualWrite(newAddress, buffer, mSize * sizeof(T));
                
            } else {
                
                VirtualRead(mAddress, buffer, newSize * sizeof(T));
                
                VirtualWrite(newAddress, buffer, newSize * sizeof(T));
            }
            
            delete(mAddress);
            
            mAddress = newAddress;
            
            mCapacity = newSize;
        }
        
        mSize = newSize;
        
        return;
    }
    
    void reserve(uint32_t newCapacity) {
        
        uint8_t buffer[newCapacity * sizeof(T)];
        
        uint32_t newAddress = new(newCapacity * sizeof(T));
        
        VirtualRead(mAddress, buffer, newCapacity * sizeof(T));
        
        VirtualWrite(newAddress, buffer, newCapacity * sizeof(T));
        
        delete(mAddress);
        
        mAddress = newAddress;
        
        mCapacity = newCapacity;
        
        if (mSize > mCapacity) 
            mSize = mCapacity;
        
        return;
    }
    
    void push_back(T item) {
        
        resize(mCapacity + 1);
        
        set(mSize-1, item);
        
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
