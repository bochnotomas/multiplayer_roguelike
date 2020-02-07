#include "buffer.hpp"
#include <iterator>

size_t Buffer::size() {
    return cur_size;
}

void Buffer::clear() {
    chunks.clear();
    cur_size = 0;
}

void Buffer::erase(size_t n) {
    size_t bytes_left = n;
    
    for(auto it = chunks.begin(); it != chunks.end();) {
        if(it->size() > bytes_left) {
            it->erase(it->begin(), std::next(it->begin(), bytes_left));
            break;
        }
        
        bytes_left -= it->size();
        it = chunks.erase(it);
        
        if(bytes_left == 0)
            break;
    }
    
    cur_size -= n - bytes_left;
}

std::vector<uint8_t> Buffer::get_bytes(size_t offset, size_t max_bytes) {
    std::vector<uint8_t> merged;
    size_t skips_left = offset;
    size_t bytes_left = max_bytes;
    
    if(bytes_left == 0 || bytes_left > size())
        bytes_left = size();
    
    merged.reserve(bytes_left);
    
    for(auto it = chunks.begin(); it != chunks.end(); it++) {
        if(skips_left > it->size()) {
            skips_left -= it->size();
            continue;
        }
        
        size_t bytes_to_read = it->size() - skips_left;
        if(bytes_to_read > bytes_left)
            bytes_to_read = bytes_left;
        
        auto offset_begin = std::next(it->begin(), skips_left);
        auto offset_end = std::next(offset_begin, bytes_to_read);
        merged.insert(merged.end(), offset_begin, offset_end);
        
        bytes_left -= bytes_to_read;
        
        if(bytes_left == 0)
            break;
        
        skips_left = 0;
    }
    
    return merged;
}

std::vector<uint8_t> Buffer::pop_bytes(size_t max_bytes) {
    std::vector<uint8_t> merged;
    size_t bytes_left = max_bytes;
    
    if(bytes_left == 0 || bytes_left > size())
        bytes_left = size();
    
    merged.reserve(bytes_left);
    
    for(auto it = chunks.begin(); it != chunks.end();) {
        if(it->size() > bytes_left) {
            auto chunk_end = std::next(it->begin(), bytes_left);
            merged.insert(merged.end(), it->begin(), chunk_end);
            it->erase(it->begin(), chunk_end);
            break;
        }
        
        bytes_left -= it->size();
        merged.insert(merged.end(), it->begin(), it->end());
        it = chunks.erase(it);
        
        if(bytes_left == 0)
            break;
    }
    
    cur_size -= merged.size();
    return merged;
}

void Buffer::insert(const std::vector<uint8_t>& bytes) {
    if(bytes.size() != 0) {
        chunks.push_back(bytes);
        cur_size += bytes.size();
    }
}

void Buffer::insert(const std::string& string) {
    if(string.size() != 0) {
        chunks.emplace_back(string.begin(), string.end());
        cur_size += string.size();
    }
}

void Buffer::insert(uint8_t byte) {
    chunks.emplace_back(1, byte);
    cur_size++;
}

void Buffer::insert(uint16_t uint16) {
    chunks.push_back({
        static_cast<uint8_t>( uint16       & 0xFF),
        static_cast<uint8_t>((uint16 >> 8) & 0xFF)
    });
    
    cur_size += 2;
}

void Buffer::insert(uint32_t uint32) {
    chunks.push_back({
        static_cast<uint8_t>( uint32        & 0xFF),
        static_cast<uint8_t>((uint32 >>  8) & 0xFF),
        static_cast<uint8_t>((uint32 >> 16) & 0xFF),
        static_cast<uint8_t>((uint32 >> 24) & 0xFF)
    });
    
    cur_size += 4;
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
    
    cur_size += 8;
}

void Buffer::get(std::vector<uint8_t>& bytes, size_t offset, size_t max_bytes) {
    bytes = get_bytes(offset, max_bytes);
}

void Buffer::get(std::string& string, size_t offset, size_t max_bytes) {
    std::vector<uint8_t> bytes = get_bytes(offset, max_bytes);
    string = std::string(bytes.begin(), bytes.end());
}

bool Buffer::get(uint8_t& byte, size_t offset) {
    if(cur_size == 0)
        return false;
    
    byte = get_bytes(offset, 1)[0];
    
    return true;
}

bool Buffer::get(uint16_t& uint16, size_t offset) {
    if(cur_size < 2)
        return false;
    
    std::vector<uint8_t> bytes = get_bytes(offset, 2);
    uint16 = static_cast<uint16_t>(bytes[0]) |
             static_cast<uint16_t>(bytes[1]) << 8;
    
    return true;
}

bool Buffer::get(uint32_t& uint32, size_t offset) {
    if(cur_size < 4)
        return false;
    
    std::vector<uint8_t> bytes = get_bytes(offset, 4);
    uint32 = static_cast<uint32_t>(bytes[0]) |
             static_cast<uint32_t>(bytes[1]) << 8 |
             static_cast<uint32_t>(bytes[2]) << 16 |
             static_cast<uint32_t>(bytes[3]) << 24;
    
    return true;
}

bool Buffer::get(uint64_t& uint64, size_t offset) {
    if(cur_size < 8)
        return false;
    
    std::vector<uint8_t> bytes = get_bytes(offset, 8);
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

void Buffer::pop(std::vector<uint8_t>& bytes, size_t max_bytes) {
    bytes = pop_bytes(max_bytes);
}

void Buffer::pop(std::string& string, size_t max_bytes) {
    std::vector<uint8_t> bytes = pop_bytes(max_bytes);
    string = std::string(bytes.begin(), bytes.end());
}

bool Buffer::pop(uint8_t& byte) {
    if(cur_size == 0)
        return false;
    
    byte = pop_bytes(1)[0];
    
    return true;
}

bool Buffer::pop(uint16_t& uint16) {
    if(cur_size < 2)
        return false;
    
    std::vector<uint8_t> bytes = pop_bytes(2);
    uint16 = static_cast<uint16_t>(bytes[0]) |
             static_cast<uint16_t>(bytes[1]) << 8;
    
    return true;
}

bool Buffer::pop(uint32_t& uint32) {
    if(cur_size < 4)
        return false;
    
    std::vector<uint8_t> bytes = pop_bytes(4);
    uint32 = static_cast<uint32_t>(bytes[0]) |
             static_cast<uint32_t>(bytes[1]) << 8 |
             static_cast<uint32_t>(bytes[2]) << 16 |
             static_cast<uint32_t>(bytes[3]) << 24;
    
    return true;
}

bool Buffer::pop(uint64_t& uint64) {
    if(cur_size < 8)
        return false;
    
    std::vector<uint8_t> bytes = pop_bytes(8);
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
