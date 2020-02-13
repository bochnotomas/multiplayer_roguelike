#ifndef ROGUELIKE_CLIENT_HPP_INCLUDED
#define ROGUELIKE_CLIENT_HPP_INCLUDED
#include "../networking/client_message.hpp"
#include "../networking/socket.hpp"
#include <mutex>

class Client {
    // Buffers and locks
    std::mutex r_lock, w_lock, s_lock;
    Buffer r_buffer, w_buffer;
    
    /// Client socket connected to server
    std::shared_ptr<Socket> client_socket;
public:
    /// Connect client to server via host and port, with a timeout
    Client(std::string host, uint16_t port, int timeout_ms);
    
    /// Receive messages, with a timeout
    void receive_messages(int timeout_ms);
    
    /// Attempt to send buffered messages. Stops sending even if not all data
    /// was sent if after timeout_ms milliseconds, unless timeout is negative
    /// where it tries forever
    void send_messages(int timeout_ms);
    
    /// Add a message to be sent to the server
    void add_message(const ClientMessage& message);
    
    /// Get messages sent to the client
    std::deque<std::unique_ptr<ClientMessage>> get_messages();
};

#endif
