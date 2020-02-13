#ifndef ROGUELIKE_SOCKET_SELECT_HPP_INCLUDED
#define ROGUELIKE_SOCKET_SELECT_HPP_INCLUDED
#include "socket.hpp"
#include <memory>

/// Type of event to be selected
enum SelectedEventType {
    Read = 1,       // There is data to be read!
    Write = 2,      // You can now send more data!
    Exceptional = 4 // There is an exceptional event! (See TCP out-of-band)
};

/// A selected event (or an event to select)
struct SelectedEvent {
    // The types of this event.
    // Note that this is an int because it is a bitmask of SelectedEventType
    int types;
    // The socket affected by this event
    std::shared_ptr<Socket> socket;
    
    SelectedEvent(int types, std::shared_ptr<Socket> socket) :
        types(types),
        socket(socket)
    {}
    
    // Check if event is of the given type. Note that an event can be of
    // multiple types
    bool is_of_type(SelectedEventType type);
};

/// A wrapper for select(). Waits for an event to happen at at least one of the
/// sockets provided. Can be re-used, assuming sockets remain valid
class SocketSelector {
    // Sockets to wait for specified event types
    std::vector<SelectedEvent> event_wait_list;
public:
    // Add an event to the waiting list. Invalidated sockets will be ignored
    void add_wait(int event_types, std::shared_ptr<Socket> socket);
    
    // Wait for an event to happen, with a timeout in milliseconds. If negative
    // this will block until an event happens. If 0, this will immediately
    // return, else, it will wait up to timeout_ms milliseconds for any event.
    // Invalidated sockets will be removed from the waiting list
    std::vector<SelectedEvent> wait(int timeout_ms);
};

#endif
