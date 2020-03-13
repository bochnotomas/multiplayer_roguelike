#include "Action.hpp"
#include <stdexcept>

Action Action::fromBytes(const std::vector<uint8_t>& data) {
    if(data.size() == 0)
        throw std::invalid_argument("Action::fromBytes: Empty input data");
    
    switch(data[0]) {
        case static_cast<uint8_t>(ActionType::Move):
            if(data.size() != 2)
                throw std::invalid_argument("Action::fromBytes: Failed to create MoveAction. Expected 2 bytes, got " + std::to_string(data.size()));
            
            return MoveAction(data[1]);
        case static_cast<uint8_t>(ActionType::UseItem):
            // TODO
            throw "Action::fromBytes: NIY - ActionType::UseItem constructor";
            break;
        default:
            throw std::invalid_argument("Action::fromBytes: Invalid ActionType byte with value " + std::to_string(data[0]));
    }
}

std::vector<uint8_t> Action::toBytes() {
    std::vector<uint8_t> bytes;
    bytes.reserve(1 + data.size());
    bytes.push_back(static_cast<uint8_t>(type));
    bytes.insert(bytes.end(), data.begin(), data.end());
    return bytes;
}

eDirection MoveAction::getDirection() {
    switch(data[0]) {
        case static_cast<uint8_t>(eDirection::STOP):
            return eDirection::STOP;
        case static_cast<uint8_t>(eDirection::LEFT):
            return eDirection::LEFT;
        case static_cast<uint8_t>(eDirection::RIGHT):
            return eDirection::RIGHT;
        case static_cast<uint8_t>(eDirection::UP):
            return eDirection::UP;
        case static_cast<uint8_t>(eDirection::DOWN):
            return eDirection::DOWN;
        default:
            return eDirection::INVALID;
    }
}
