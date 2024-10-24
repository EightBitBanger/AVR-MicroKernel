#ifndef __STRING_ALLOCATOR_
#define __STRING_ALLOCATOR_


class string {
    
public:
    
    string();
    string(const string& str);
    
    ~string();
    
    uint32_t size(void);
    
    void resize(uint32_t newSize);
    
    void data(uint8_t* buffer, uint32_t length);
    
    void operator= (uint8_t* source);
    
    uint8_t operator[] (unsigned int const i);
    
private:
    
    uint32_t mAddress;
    
    uint32_t mLength;
    
};

#endif
