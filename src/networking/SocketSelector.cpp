#include "SocketSelector.hpp"

#ifdef ROGUELIKE_UNIX
    #include <sys/select.h>
#endif

bool SelectedEvent::isOfType(SelectedEventType type) {
    return types & type;
}


void SocketSelector::addWait(int eventTypes, std::shared_ptr<Socket> socket) {
    // Skip invalidated sockets
    if(!socket->isValid())
        return;
    
    // If the socket is already in the wait list, update event types
    for(auto it = eventWaitList.begin(); it != eventWaitList.end(); it++) {
        if(it->socket.get() == socket.get()) {
            it->types |= eventTypes;
            return;
        }
    }
    
    // Panic if the socket limit is reached (Unix limit used)
    if(eventWaitList.size() == FD_SETSIZE)
        throw SocketException("SocketSelector::addWait: socket select limit (" + std::to_string(FD_SETSIZE) + ") reached");
    
    // Else, insert into the wait list
    eventWaitList.emplace_back(eventTypes, socket);
}

std::vector<SelectedEvent> SocketSelector::wait(int timeoutMs) {
    // Setup read, write and except wait list...
    fd_set readFds, writeFds, exceptFds;
    bool readSome = false,
         writeSome = false,
         exceptSome = false;
    
    FD_ZERO(&readFds);
    FD_ZERO(&writeFds);
    FD_ZERO(&exceptFds);
    
    // ... and pick biggest socket file descriptor if on Unix
    int nfds = -1;
    
    for(auto it = eventWaitList.begin(); it != eventWaitList.end();) {
        // Remove invalidated sockets from waiting list
        if(!it->socket->isValid()) {
            it = eventWaitList.erase(it);
            continue;
        }
        
        auto rawSock = it->socket->rawSock;
        
        #ifdef ROGUELIKE_UNIX
        if(it->types != 0 && rawSock > nfds)
            nfds = rawSock;
        #endif
        
        if((it->types & SelectedEventType::Read) != 0) {
            FD_SET(rawSock, &readFds);
            readSome = true;
        }
        
        if((it->types & SelectedEventType::Write) != 0) {
            FD_SET(rawSock, &writeFds);
            writeSome = true;
        }
        
        if((it->types & SelectedEventType::Exceptional) != 0) {
            FD_SET(rawSock, &exceptFds);
            exceptSome = true;
        }
        
        it++;
    }
    
    // Setup pointers to pass to select
    fd_set *rfdsPtr = readSome ? &readFds : nullptr,
           *wfdsPtr = writeSome ? &writeFds : nullptr,
           *efdsPtr = exceptSome ? &exceptFds : nullptr;
    
    // Abort if there are no events to select
    std::vector<SelectedEvent> selectedEvents;
    if(!(rfdsPtr || wfdsPtr || efdsPtr))
        return selectedEvents;
    
    // Select...
    int result;
    if(timeoutMs < 0) {
        // ...blocking
        result = select(nfds + 1, rfdsPtr, wfdsPtr, efdsPtr, nullptr);
    }
    else {
        // ... non-blocking with timeout
        // Setup timeout
        timeval tv;
        tv.tv_sec = timeoutMs / 1000;
        tv.tv_usec = (timeoutMs % 1000) * 1000;
        
        result = select(nfds + 1, rfdsPtr, wfdsPtr, efdsPtr, &tv);
    }
    
    // Throw exception on error
    if(result == SOCKET_ERROR)
        throw SocketException::fromErrno("SocketSelector::wait: ");
    
    // Parse events
    for(auto it = eventWaitList.begin(); it != eventWaitList.end(); it++) {
        // Get event types...
        int eventTypes = 0;
        auto rawSock = it->socket->rawSock;
        
        // ... Read
        if(rfdsPtr && FD_ISSET(rawSock, rfdsPtr))
            eventTypes |= SelectedEventType::Read;
        
        // ... Write
        if(wfdsPtr && FD_ISSET(rawSock, wfdsPtr))
            eventTypes |= SelectedEventType::Write;
        
        // ... Exceptional
        if(efdsPtr && FD_ISSET(rawSock, efdsPtr))
            eventTypes |= SelectedEventType::Exceptional;
        
        // Add to selected events
        selectedEvents.emplace_back(eventTypes, it->socket);
    }
    
    return selectedEvents;
}
