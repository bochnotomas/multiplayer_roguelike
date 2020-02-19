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
    chunks.push_back({
        static_cast<uint8_t>( uint16       & 0xFF),
        static_cast<uint8_t>((uint16 >> 8) & 0xFF)
    });
    
    curSize += 2;
}

void Buffer::insert(uint32_t uint32) {
    chunks.push_back({
        static_cast<uint8_t>( uint32        & 0xFF),
        static_cast<uint8_t>((uint32 >>  8) & 0xFF),
        static_cast<uint8_t>((uint32 >> 16) & 0xFF),
        static_cast<uint8_t>((uint32 >> 24) & 0xFF)
    });
    
    curSize += 4;
}

void Buffer::insert(uint64_t uint64) {
    chunks.push_back({
        static_cast<uint8_t>( uint64        & 0xFF),
        static_cast<uint8_t>((uint64 >>  8) & 0xFF),
        static_cast<uint8_t>((uint64 >> 16) & 0xFF),
        static_cast<uint8_t>((uint64 >> 24) & 0xFF),
        static_cast<uint8_t>((uint64 >> 32) & 0xFF),
        static_cast<uint8_t>((uint64 >> 40) & 0xFF),
        static_cast<uint8_t>((uint64 >> 48) & 0xFF),
        static_cast<uint8_t>((uint64 >> 56) & 0xFF)
    });
    
    curSize += 8;
}

void Buffer::get(std::vector<uint8_t>& bytes, size_t byteCount, size_t offset) {
    bytes = getBytes(byteCount, offset);
}

void Buffer::get(std::string& string, size_t byteCount, size_t offset) {
    std::vector<uint8_t> bytes = getBytes(byteCount, offset);
    string = std::string(bytes.begin(), bytes.end());
}

bool Buffer::get(uint8_t& byte, size_t offset) {
    if(curSize == 0)
        return false;
    
    byte = getBytes(1, offset)[0];
    
    return true;
}

bool Buffer::get(uint16_t& uint16, size_t offset) {
    if(curSize < 2)
        return false;
    
    std::vector<uint8_t> bytes = getBytes(2, offset);
    uint16 = static_cast<uint16_t>(bytes[0]) |
             static_cast<uint16_t>(bytes[1]) << 8;
    
    return true;
}

bool Buffer::get(uint32_t& uint32, size_t offset) {
    if(curSize < 4)
        return false;
    
    std::vector<uint8_t> bytes = getBytes(4, offset);
    uint32 = static_cast<uint32_t>(bytes[0]) |
             static_cast<uint32_t>(bytes[1]) << 8 |
             static_cast<uint32_t>(bytes[2]) << 16 |
             static_cast<uint32_t>(bytes[3]) << 24;
    
    return true;
}

bool Buffer::get(uint64_t& uint64, size_t offset) {
    if(curSize < 8)
        return false;
    
    std::vector<uint8_t> bytes = getBytes(8, offset);
    uint64 = static_cast<uint64_t>(bytes[0]) |
             static_cast<uint64_t>(bytes[1]) << 8 |
             static_cast<uint64_t>(bytes[2]) << 16 |
             static_cast<uint64_t>(bytes[3]) << 24 |
             static_cast<uint64_t>(bytes[4]) << 32 |
             static_cast<uint64_t>(bytes[5]) << 40 |
             static_cast<uint64_t>(bytes[6]) << 48 |
             static_cast<uint64_t>(bytes[7]) << 56;
    
    return true;
}

void Buffer::pop(std::vector<uint8_t>& bytes, size_t byteCount) {
    bytes = popBytes(byteCount);
}

void Buffer::pop(std::string& string, size_t byteCount) {
    std::vector<uint8_t> bytes = popBytes(byteCount);
    string = std::string(bytes.begin(), bytes.end());
}

bool Buffer::pop(uint8_t& byte) {
    if(curSize == 0)
        return false;
    
    byte = popBytes(1)[0];
    
    return true;
}

bool Buffer::pop(uint16_t& uint16) {
    if(curSize < 2)
        return false;
    
    std::vector<uint8_t> bytes = popBytes(2);
    uint16 = static_cast<uint16_t>(bytes[0]) |
             static_cast<uint16_t>(bytes[1]) << 8;
    
    return true;
}

bool Buffer::pop(uint32_t& uint32) {
    if(curSize < 4)
        return false;
    
    std::vector<uint8_t> bytes = popBytes(4);
    uint32 = static_cast<uint32_t>(bytes[0]) |
             static_cast<uint32_t>(bytes[1]) << 8 |
             static_cast<uint32_t>(bytes[2]) << 16 |
             static_cast<uint32_t>(bytes[3]) << 24;
    
    return true;
}

bool Buffer::pop(uint64_t& uint64) {
    if(curSize < 8)
        return false;
    
    std::vector<uint8_t> bytes = popBytes(8);
    uint64 = static_cast<uint64_t>(bytes[0]) |
             static_cast<uint64_t>(bytes[1]) << 8 |
             static_cast<uint64_t>(bytes[2]) << 16 |
             static_cast<uint64_t>(bytes[3]) << 24 |
             static_cast<uint64_t>(bytes[4]) << 32 |
             static_cast<uint64_t>(bytes[5]) << 40 |
             static_cast<uint64_t>(bytes[6]) << 48 |
             static_cast<uint64_t>(bytes[7]) << 56;
    
    return true;
}
