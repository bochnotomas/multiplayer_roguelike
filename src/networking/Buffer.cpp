#include "Buffer.hpp"
#include <stdexcept>
#include <iterator>

size_t Buffer::size() {
    return curSize;
}

void Buffer::clear() {
    chunks.clear();
    curSize = 0;
}

void Buffer::erase(size_t byteCount) {
    if(byteCount > curSize)
        throw std::out_of_range("Buffer::erase(" + std::to_string(byteCount) + ") called but curSize is " + std::to_string(curSize));
    
    curSize -= byteCount;
    
    for(auto it = chunks.begin(); it != chunks.end();) {
        if(it->size() > byteCount) {
            it->erase(it->begin(), std::next(it->begin(), byteCount));
            break;
        }
        
        byteCount -= it->size();
        it = chunks.erase(it);
        
        if(byteCount == 0)
            break;
    }
}

std::vector<uint8_t> Buffer::getBytes(size_t byteCount, size_t offset) {
    if((byteCount + offset) > curSize)
        throw std::out_of_range("Buffer::getBytes(" + std::to_string(byteCount) + ", " + std::to_string(offset) + ") called but curSize is " + std::to_string(curSize));
    
    std::vector<uint8_t> merged;
    merged.reserve(byteCount);
    
    for(auto it = chunks.begin(); it != chunks.end(); it++) {
        if(offset >= it->size()) {
            offset -= it->size();
            continue;
        }
        
        size_t bytesToRead = it->size() - offset;
        if(bytesToRead > byteCount)
            bytesToRead = byteCount;
        
        auto offsetBegin = std::next(it->begin(), offset);
        auto offsetEnd = std::next(offsetBegin, bytesToRead);
        merged.insert(merged.end(), offsetBegin, offsetEnd);
        
        byteCount -= bytesToRead;
        
        if(byteCount == 0)
            break;
        
        offset = 0;
    }
    
    return merged;
}

std::vector<uint8_t> Buffer::popBytes(size_t byteCount) {
    if(byteCount > curSize)
        throw std::out_of_range("Buffer::popBytes(" + std::to_string(byteCount) + ") called but curSize is " + std::to_string(curSize));
    
    curSize -= byteCount;
    
    std::vector<uint8_t> merged;
    merged.reserve(byteCount);
    
    for(auto it = chunks.begin(); it != chunks.end();) {
        if(it->size() > byteCount) {
            auto chunkEnd = std::next(it->begin(), byteCount);
            merged.insert(merged.end(), it->begin(), chunkEnd);
            it->erase(it->begin(), chunkEnd);
            break;
        }
        
        byteCount -= it->size();
        merged.insert(merged.end(), it->begin(), it->end());
        it = chunks.erase(it);
        
        if(byteCount == 0)
            break;
    }
    
    return merged;
}

void Buffer::orUIntToCBuffer(uintmax_t integer, size_t n, uint8_t* cBuffer) {
    for(auto i = 0; i < n; i++) {
        // OR byte into array
        cBuffer[i] |= static_cast<uint8_t>(integer & 0xFF);
        
        // Shift integer right one byte
        integer >>= 8;
    }
}

void Buffer::orIntToCBuffer(intmax_t integer, size_t n, uint8_t* cBuffer) {
    // Special case for most negative number. Only set MSB
    if(integer == INTMAX_MIN) {
        cBuffer[n - 1] |= 0b10000000;
        return;
    }
    
    // Encode as 2's complement integer. Extra padding will be safely truncated
    if(integer < 0)
        orUIntToCBuffer(~static_cast<uintmax_t>(-integer) + 1, n, cBuffer);
    else
        orUIntToCBuffer(static_cast<uintmax_t>(integer), n, cBuffer);
}

void Buffer::insertUInt(uintmax_t integer, size_t n) {
    std::vector<uint8_t> bytes(n, 0);
    orUIntToCBuffer(integer, n, bytes.data());
    chunks.push_back(bytes);
    curSize += n;
}

void Buffer::insertInt(intmax_t integer, size_t n) {
    std::vector<uint8_t> bytes(n, 0);
    orIntToCBuffer(integer, n, bytes.data());
    chunks.push_back(bytes);
    curSize += n;
}

uintmax_t Buffer::cBufferToUInt(size_t n, uint8_t* cBuffer) {
    uintmax_t integer = 0;
    for(auto i = 0; i < n; i++)
        integer += static_cast<uintmax_t>(cBuffer[i]) << (i * 8);
    return integer;
}

intmax_t Buffer::cBufferToInt(size_t n, uint8_t* cBuffer) {
    intmax_t integer = 0;
    
    // Decode most significant byte
    uint8_t msByte = cBuffer[n - 1];
    
    // Negative bit
    if(msByte & 0b10000000)
        integer = -(static_cast<uintmax_t>(1) << (n * 8 - 1));
    
    // Positive bits
    integer += static_cast<uintmax_t>(msByte & 0b01111111) << ((n - 1) * 8);
    
    // Decode positive bits of integer
    for(auto i = 0; i < n - 1; i++)
        integer += static_cast<uintmax_t>(cBuffer[i]) << (i * 8);
    
    return integer;
}

template<typename T> T Buffer::getUInt(size_t offset) {
    auto bytes = getBytes(sizeof(T), offset);
    return static_cast<T>(cBufferToUInt(sizeof(T), bytes.data()));
}

template<typename T> T Buffer::getInt(size_t offset) {
    auto bytes = getBytes(sizeof(T), offset);
    return static_cast<T>(cBufferToInt(sizeof(T), bytes.data()));
}

template<typename T> T Buffer::popUInt() {
    auto bytes = popBytes(sizeof(T));
    return static_cast<T>(cBufferToUInt(sizeof(T), bytes.data()));
}

template<typename T> T Buffer::popInt() {
    auto bytes = popBytes(sizeof(T));
    return static_cast<T>(cBufferToInt(sizeof(T), bytes.data()));
}

void Buffer::insert(const std::vector<uint8_t>& bytes) {
    if(bytes.size() != 0) {
        chunks.push_back(bytes);
        curSize += bytes.size();
    }
}

void Buffer::insert(const std::string& string) {
    if(string.size() != 0) {
        chunks.emplace_back(string.begin(), string.end());
        curSize += string.size();
    }
}

void Buffer::insert(uint8_t byte) {
    chunks.emplace_back(1, byte);
    curSize++;
}

void Buffer::insert(uint16_t uint16) {
    insertUInt(uint16, 2);
}

void Buffer::insert(uint32_t uint32) {
    insertUInt(uint32, 4);
}

void Buffer::insert(uint64_t uint64) {
    insertUInt(uint64, 8);
}

void Buffer::insert(int8_t int8) {
    insertInt(int8, 1);
}

void Buffer::insert(int16_t int16) {
    insertInt(int16, 2);
}

void Buffer::insert(int32_t int32) {
    insertInt(int32, 4);
}

void Buffer::insert(int64_t int64) {
    insertInt(int64, 8);
}

void Buffer::get(std::vector<uint8_t>& bytes, size_t byteCount, size_t offset) {
    bytes = getBytes(byteCount, offset);
}

void Buffer::get(std::string& string, size_t byteCount, size_t offset) {
    std::vector<uint8_t> bytes = getBytes(byteCount, offset);
    string = std::string(bytes.begin(), bytes.end());
}

void Buffer::get(uint8_t& byte, size_t offset) {
    byte = getBytes(1, offset)[0];
}

void Buffer::get(uint16_t& uint16, size_t offset) {
    uint16 = getUInt<uint16_t>(offset);
}

void Buffer::get(uint32_t& uint32, size_t offset) {
    uint32 = getUInt<uint32_t>(offset);
}

void Buffer::get(uint64_t& uint64, size_t offset) {
    uint64 = getUInt<uint64_t>(offset);
}

void Buffer::get(int8_t& int8, size_t offset) {
    int8 = getInt<int8_t>(offset);
}

void Buffer::get(int16_t& uint16, size_t offset) {
    uint16 = getInt<int16_t>(offset);
}

void Buffer::get(int32_t& uint32, size_t offset) {
    uint32 = getInt<int32_t>(offset);
}

void Buffer::get(int64_t& uint64, size_t offset) {
    uint64 = getInt<int64_t>(offset);
}

void Buffer::pop(std::vector<uint8_t>& bytes, size_t byteCount) {
    bytes = popBytes(byteCount);
}

void Buffer::pop(std::string& string, size_t byteCount) {
    std::vector<uint8_t> bytes = popBytes(byteCount);
    string = std::string(bytes.begin(), bytes.end());
}

void Buffer::pop(uint8_t& byte) {
    byte = popBytes(1)[0];
}

void Buffer::pop(uint16_t& uint16) {
    uint16 = popUInt<uint16_t>();
}

void Buffer::pop(uint32_t& uint32) {
    uint32 = popUInt<uint32_t>();
}

void Buffer::pop(uint64_t& uint64) {
    uint64 = popUInt<uint64_t>();
}

void Buffer::pop(int8_t& int8) {
    int8 = popInt<int8_t>();
}

void Buffer::pop(int16_t& int16) {
    int16 = popInt<int16_t>();
}

void Buffer::pop(int32_t& int32) {
    int32 = popInt<int32_t>();
}

void Buffer::pop(int64_t& int64) {
    int64 = popInt<int64_t>();
}
