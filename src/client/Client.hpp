#ifndef ROGUELIKE_CLIENT_HPP_INCLUDED
#define ROGUELIKE_CLIENT_HPP_INCLUDED
#include "../networking/ClientMessage.hpp"
#include "../networking/Socket.hpp"
#include <mutex>

class Client {
    // Buffers and locks
    std::mutex rLock, wLock, sLock;
    Buffer rBuffer, wBuffer;
    
    /// Client socket connected to server
    std::shared_ptr<Socket> clientSocket;
public:
    /// Connect client to server via host and port, with a timeout
    Client(std::string host, uint16_t port, int timeoutMs);
    
    /// Destructor
    virtual ~Client();
    
    /// Receive messages, with a timeout
    void receiveMessages(int timeoutMs);
    
    /// Attempt to send buffered messages. Stops sending even if not all data
    /// was sent if after timeoutMs milliseconds, unless timeout is negative
    /// where it tries forever
    void sendMessages(int timeoutMs);
    
    /// Add a message to be sent to the server
    void addMessage(const ClientMessage& message);
    
    /// Get messages sent to the client
    std::deque<std::unique_ptr<ClientMessage>> getMessages();
    
    /// Check if client socket is still open
    bool isSocketOpen();
};

#endif
