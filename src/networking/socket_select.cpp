#include "socket_select.hpp"

#ifdef ROGUELIKE_UNIX
    #include <sys/select.h>
#endif

bool SelectedEvent::is_of_type(SelectedEventType type) {
    return types & type;
}


void SocketSelector::add_wait(int event_types, std::shared_ptr<Socket> socket) {
    // Skip invalidated sockets
    if(!socket->is_valid())
        return;
    
    // If the socket is already in the wait list, update event types
    for(auto it = event_wait_list.begin(); it != event_wait_list.end(); it++) {
        if(it->socket.get() == socket.get()) {
            it->types |= event_types;
            return;
        }
    }
    
    // Panic if the socket limit is reached (Unix limit used)
    if(event_wait_list.size() == FD_SETSIZE)
        throw SocketException("SocketSelector::add_wait: socket select limit (" + std::to_string(FD_SETSIZE) + ") reached");
    
    // Else, insert into the wait list
    event_wait_list.emplace_back(event_types, socket);
}

std::vector<SelectedEvent> SocketSelector::wait(int timeout_ms) {
    // Setup read, write and except wait list...
    fd_set read_fds, write_fds, except_fds;
    bool read_some = false,
         write_some = false,
         except_some = false;
    
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&except_fds);
    
    // ... and pick biggest socket file descriptor if on Unix
    int nfds = -1;
    
    for(auto it = event_wait_list.begin(); it != event_wait_list.end();) {
        // Remove invalidated sockets from waiting list
        if(!it->socket->is_valid()) {
            it = event_wait_list.erase(it);
            continue;
        }
        
        auto raw_sock = it->socket->raw_sock;
        
        #ifdef ROGUELIKE_UNIX
        if(it->types != 0 && raw_sock > nfds)
            nfds = raw_sock;
        #endif
        
        if((it->types & SelectedEventType::Read) != 0) {
            FD_SET(raw_sock, &read_fds);
            read_some = true;
        }
        
        if((it->types & SelectedEventType::Write) != 0) {
            FD_SET(raw_sock, &write_fds);
            write_some = true;
        }
        
        if((it->types & SelectedEventType::Exceptional) != 0) {
            FD_SET(raw_sock, &except_fds);
            except_some = true;
        }
        
        it++;
    }
    
    // Setup pointers to pass to select
    fd_set *rfds_ptr = read_some ? &read_fds : nullptr,
           *wfds_ptr = write_some ? &write_fds : nullptr,
           *efds_ptr = except_some ? &except_fds : nullptr;
    
    // Abort if there are no events to select
    std::vector<SelectedEvent> selected_events;
    if(!(rfds_ptr || wfds_ptr || efds_ptr))
        return selected_events;
    
    // Select...
    int result;
    if(timeout_ms < 0) {
        // ...blocking
        result = select(nfds + 1, rfds_ptr, wfds_ptr, efds_ptr, nullptr);
    }
    else {
        // ... non-blocking with timeout
        // Setup timeout
        timeval tv = {
            .tv_sec = timeout_ms / 1000,
            .tv_usec = (timeout_ms % 1000) * 1000
        };
        
        result = select(nfds + 1, rfds_ptr, wfds_ptr, efds_ptr, &tv);
    }
    
    // Throw exception on error
    if(result == SOCKET_ERROR)
        throw SocketException::from_errno("SocketSelector::wait: ");
    
    // Parse events
    for(auto it = event_wait_list.begin(); it != event_wait_list.end(); it++) {
        // Get event types...
        int event_types = 0;
        auto raw_sock = it->socket->raw_sock;
        
        // ... Read
        if(rfds_ptr && FD_ISSET(raw_sock, rfds_ptr))
            event_types |= SelectedEventType::Read;
        
        // ... Write
        if(wfds_ptr && FD_ISSET(raw_sock, wfds_ptr))
            event_types |= SelectedEventType::Write;
        
        // ... Exceptional
        if(efds_ptr && FD_ISSET(raw_sock, efds_ptr))
            event_types |= SelectedEventType::Exceptional;
        
        // Add to selected events
        selected_events.emplace_back(event_types, it->socket);
    }
    
    return selected_events;
}
