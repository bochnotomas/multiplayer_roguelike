#ifndef ROGUELIKE_BUFFER_HPP_INCLUDED
#define ROGUELIKE_BUFFER_HPP_INCLUDED
#include <cstdint>
#include <string>
#include <vector>
#include <array>
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
    
    /// ORs a little-endian n-byte unsigned integer to a given C byte buffer
    void orUIntToCBuffer(uintmax_t integer, size_t n, uint8_t* cBuffer);
    
    /// ORs a little-endian n-byte signed integer to a given C byte buffer
    void orIntToCBuffer(intmax_t integer, size_t n, uint8_t* cBuffer);
    
    /// Inserts a little-endian n-byte unsigned integer to the buffer
    inline void insertUInt(uintmax_t integer, size_t n);
    
    /// Inserts a little-endian n-byte signed integer to the buffer
    inline void insertInt(intmax_t integer, size_t n);
    
    /// Gets a little-endian n-byte unsigned integer from a given C byte buffer
    uintmax_t cBufferToUInt(size_t n, uint8_t* cBuffer);
    
    /// Gets a little-endian n-byte signed integer from a given C byte buffer
    intmax_t cBufferToInt(size_t n, uint8_t* cBuffer);
    
    /// Gets a little-endian n-byte unsigned integer from the buffer
    template<typename T> inline T getUInt(size_t offset);
    
    /// Gets a little-endian n-byte signed integer from the buffer
    template<typename T> inline T getInt(size_t offset);
    
    /// Pops a little-endian n-byte unsigned integer from the buffer
    template<typename T> inline T popUInt();
    
    /// Pops a little-endian n-byte signed integer from the buffer
    template<typename T> inline T popInt();
    
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
    
    // Inserts a little-endian int8_t to the buffer. Very inefficient, 1 chunk
    // per byte
    void insert(int8_t int8);
    
    // Inserts a little-endian int16_t to the buffer
    void insert(int16_t int16);
    
    // Inserts a little-endian int32_t to the buffer
    void insert(int32_t int32);
    
    // Inserts a little-endian int64_t to the buffer
    void insert(int64_t int64);
    
    // Gets byteCount bytes from the buffer, with an offset
    void get(std::vector<uint8_t>& bytes, size_t byteCount, size_t offset = 0);
    
    // Gets a string with byteCount bytes from the buffer, with an offset
    void get(std::string& string, size_t byteCount, size_t offset = 0);
    
    // Gets a byte from the buffer, with an offset
    void get(uint8_t& byte, size_t offset = 0);
    
    // Gets a little-endian uint16_t from the buffer, with an offset
    void get(uint16_t& uint16, size_t offset = 0);
    
    // Gets a little-endian uint32_t from the buffer, with an offset
    void get(uint32_t& uint32, size_t offset = 0);
    
    // Gets a little-endian uint64_t from the buffer, with an offset
    void get(uint64_t& uint64, size_t offset = 0);
    
    // Gets a little-endian int8_t from the buffer, with an offset
    void get(int8_t& int8, size_t offset = 0);
    
    // Gets a little-endian int16_t from the buffer, with an offset
    void get(int16_t& int16, size_t offset = 0);
    
    // Gets a little-endian int32_t from the buffer, with an offset
    void get(int32_t& int32, size_t offset = 0);
    
    // Gets a little-endian int64_t from the buffer, with an offset
    void get(int64_t& int64, size_t offset = 0);
    
    // Gets byteCount bytes from the buffer, removing the read data
    void pop(std::vector<uint8_t>& bytes, size_t byteCount);
    
    // Gets a string with byteCount bytes from the buffer, removing the read
    // data
    void pop(std::string& string, size_t byteCount);
    
    // Gets a byte from the buffer, removing the read data
    void pop(uint8_t& byte);
    
    // Gets a little-endian uint16_t from the buffer, removing the read data
    void pop(uint16_t& uint16);
    
    // Gets a little-endian uint32_t from the buffer, removing the read data
    void pop(uint32_t& uint32);
    
    // Gets a little-endian uint64_t from the buffer, removing the read data
    void pop(uint64_t& uint64);
    
    // Gets a little-endian int8_t from the buffer, removing the read data
    void pop(int8_t& int8);
    
    // Gets a little-endian int16_t from the buffer, removing the read data
    void pop(int16_t& int16);
    
    // Gets a little-endian int32_t from the buffer, removing the read data
    void pop(int32_t& int32);
    
    // Gets a little-endian int64_t from the buffer, removing the read data
    void pop(int64_t& int64);
};

#endif
