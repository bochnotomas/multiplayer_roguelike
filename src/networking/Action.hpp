#ifndef ROGUELIKE_ACTION_HPP_INCLUDED
#define ROGUELIKE_ACTION_HPP_INCLUDED
#include "Direction.hpp"
#include "Buffer.hpp"
#include <cstdint>
#include <vector>

enum class ActionType {
    Move = 0,
    UseItem = 1
};

class Action {
protected:
    // Action raw data
    std::vector<uint8_t> data;
public:
    // Action type
    const ActionType type;
    
    Action(ActionType type, std::vector<uint8_t> data) :
        data(data),
        type(type)
    {}
    
    virtual ~Action() = default;
    
    // Get action from bytes. Throws std::invalid_argument when the input data
    // is invalid
    static Action fromBytes(const std::vector<uint8_t>& data);
    
    // Get bytes from action
    std::vector<uint8_t> toBytes() const;
};

class MoveAction : public Action {
public:
    // Get direction
    eDirection getDirection();
    
    MoveAction(eDirection direction) :
        Action(ActionType::Move, {static_cast<uint8_t>(direction)})
    {}

    MoveAction(uint8_t rawDirection) :
        Action(ActionType::Move, {rawDirection})
    {}
};

class UseItemAction : public Action {
public:
    // Get inventory position. -1 when invalid
    int getItem();
    
    UseItemAction(int64_t itemPos) :
        Action(ActionType::UseItem, {})
    {
        Buffer buffer;
        buffer.insert(itemPos);
        buffer.get(data, buffer.size());
    }
};

#endif
