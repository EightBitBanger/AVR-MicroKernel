#ifndef __ARRAY_ALLOCATOR_
#define __ARRAY_ALLOCATOR_


class array {
    
public:
    
    array();
    array(uint32_t size);
    array(const array& arr);
    ~array();
    
    uint8_t operator[](unsigned int const i);
    //const uint8_t& operator[](unsigned int const i) const;
    
    void set(uint32_t const i, uint8_t item);
    
    uint8_t get(uint32_t const i);
    
    bool empty(void);
    
    uint32_t size(void);
    
    uint32_t max_size(void);
    
    void resize(uint32_t newSize);
    
    void reserve(uint32_t newCapacity);
    
    void push_back(uint8_t item);
    
private:
    
    uint32_t mSize;
    uint32_t mCapacity;
    uint32_t mAddress;
    
};


#endif





/*



#ifndef __ARRAY_ALLOCATOR_
#define __ARRAY_ALLOCATOR_

template<typename T, uint32_t N>
    
class array {
    
public:
    
    array();
    array(const array& arr);
    array& operator=(const array& arr);
    ~array();
    
    
    T& operator[](unsigned int const i); // Get
    const T& operator[](unsigned int const i) const; // Set
    
    void set(unsigned int const i, T item);
    T get(unsigned int const i) const;
    
    uint32_t size() const;
    uint32_t capacity() const;
    
    void resize(uint32_t newSize);
    void reserve(uint32_t newCapacity);
    void push_back(T item);
    
private:
    
    uint32_t mSize;
    uint32_t mCapacity;
    uint32_t mAddress;
    
    uint8_t mBuffer[sizeof(T)];
    
    void copy_from(const array& arr);
    
};

#include "array.tpp"

#endif

*/

