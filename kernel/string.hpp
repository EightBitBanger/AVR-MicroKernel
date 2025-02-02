#ifndef __STRING_ALLOCATOR_
#define __STRING_ALLOCATOR_


class string {
    
public:
    
    string();
    string(uint32_t size);
    string(const string& arr);
    ~string();
    
    uint8_t& operator[](unsigned int const i);
    const uint8_t& operator[](unsigned int const i) const;
    
    void set(uint32_t i, uint8_t item);
    uint8_t get(uint32_t i);
    
    bool empty(void);
    uint32_t size(void);
    uint32_t max_size(void);
    void resize(uint32_t newSize);
    void reserve(uint32_t newCapacity);
    
private:
    uint32_t mSize;
    uint32_t mCapacity;
    uint32_t mAddress;
    uint32_t mDummyIndex;
    mutable uint8_t mDummy;
    
    void updateBuffer(uint32_t index, uint8_t value);
    uint8_t readBuffer(uint32_t index) const;
};

#endif
