#ifndef __ARRAY_ALLOCATOR_
#define __ARRAY_ALLOCATOR_


class array {
    
public:
    
    array();
    
    array(uint32_t size);
    
    array(const array& arr);
    
    ~array();
    
    uint8_t operator[](unsigned int const i);
    
    void set(unsigned int const i, uint8_t item);
    
    uint8_t get(unsigned int const i);
    
    uint32_t size(void);
    
    uint32_t capacity(void);
    
    void resize(uint32_t newSize);
    
    void reserve(uint32_t newCapacity);
    
    void push_back(uint8_t item);
    
private:
    
    uint32_t mSize;
    uint32_t mCapacity;
    uint32_t mAddress;
    
};


#endif
