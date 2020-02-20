#ifndef ROGUELIKE_BUFFER_HPP_INCLUDED
#define ROGUELIKE_BUFFER_HPP_INCLUDED
#include <cstdint>
#include <string>
#include <vector>
#include <deque>

class Buffer {
    /// Chunks of data. Merge with pop() or get()
    std::deque<std::vector<uint8_t>> chunks;
    
    /// Current buffer size
    size_t curSize = 0;
    
    /// Gets data from the buffer, with an offset
    std::vector<uint8_t> getBytes(size_t byteCount, size_t offset);
    
    /// Gets data from the buffer, removing read data
    std::vector<uint8_t> popBytes(size_t byteCount);
    
public:
    /// Current size of buffer
    size_t size();
    
    /// Clear buffer
    void clear();
    
    /// Erase byteCount bytes
    void erase(size_t byteCount);
    
    // Inserts bytes to the buffer
    void insert(const std::vector<uint8_t>& bytes);
    
    // Inserts a string to the buffer
    void insert(const std::string& string);
    
    // Inserts a byte to the buffer. Very inefficient, 1 chunk per byte
    void insert(uint8_t byte);
    
    // Inserts a little-endian uint16_t to the buffer
    void insert(uint16_t uint16);
    
    // Inserts a little-endian uint32_t to the buffer
    void insert(uint32_t uint32);
    
    // Inserts a little-endian uint64_t to the buffer
    void insert(uint64_t uint64);
    
    // Gets byteCount bytes from the buffer, with an offset
    void get(std::vector<uint8_t>& bytes, size_t byteCount, size_t offset = 0);
    
    // Gets a string with byteCount bytes from the buffer, with an offset
    void get(std::string& string, size_t byteCount, size_t offset = 0);
    
    // Gets a byte from the buffer, with an offset
    bool get(uint8_t& byte, size_t offset = 0);
    
    // Gets a little-endian uint16_t from the buffer, with an offset
    bool get(uint16_t& uint16, size_t offset = 0);
    
    // Gets a little-endian uint32_t from the buffer, with an offset
    bool get(uint32_t& uint32, size_t offset = 0);
    
    // Gets a little-endian uint64_t from the buffer, with an offset
    bool get(uint64_t& uint64, size_t offset = 0);
    
    // Gets byteCount bytes from the buffer, removing the read data
    void pop(std::vector<uint8_t>& bytes, size_t byteCount);
    
    // Gets a string with byteCount bytes from the buffer, removing the read
    // data
    void pop(std::string& string, size_t byteCount);
    
    // Gets a byte from the buffer, removing the read data
    bool pop(uint8_t& byte);
    
    // Gets a little-endian uint16_t from the buffer, removing the read data
    bool pop(uint16_t& uint16);
    
    // Gets a little-endian uint32_t from the buffer, removing the read data
    bool pop(uint32_t& uint32);
    
    // Gets a little-endian uint64_t from the buffer, removing the read data
    bool pop(uint64_t& uint64);
};

#endif