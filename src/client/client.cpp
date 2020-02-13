#include "../networking/socket_select.hpp"
#include "client.hpp"
#include <chrono>

Client::Client(std::string host, uint16_t port, int timeout_ms) :
    // Open connection socket
    client_socket(new Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
{
    // Resolve host
    auto addresses = Socket::resolve(host);
    
    // Connect to first available socket
    SocketSelector selector;
    for(auto it = addresses.begin(); it != addresses.end(); it++) {
        try {
            // Create socket
            std::shared_ptr<Socket> candidate_socket(new Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
            
            // Connect
            candidate_socket->connect(AF_INET, *it, port);
            
            // Add to selector. Wait for writing to become available
            selector.add_wait(SelectedEventType::Write, candidate_socket);
        }
        catch(SocketException){}; // Ignore exceptions, just don't connect
    }
    
    // Wait for any connection to be accepted
    auto events = selector.wait(timeout_ms);
    
    // Parse events, accepting first available connection. All other sockets
    // are automatically closed
    std::shared_ptr<Socket> chosen_candidate = nullptr;
    for(auto it = events.begin(); it != events.end(); it++) {
        // Pick first available connection
        if(it->is_of_type(SelectedEventType::Write) && it->socket->is_valid()) {
            chosen_candidate = std::move(it->socket);
            break;
        }
    }
    
    if(chosen_candidate == nullptr)
        throw SocketException("Failed to connect to server: timed out");
    
    // Close connection socket automatically and replace with new connection
    client_socket = std::move(chosen_candidate);
    
    // Make socket non-blocking as we will be reading and writing from now on
    client_socket->set_blocking(false);
}

void Client::receive_messages(int timeout_ms) {
    // Lock socket
    const std::lock_guard<std::mutex> s_lock_guard(s_lock);
        
    std::deque<std::shared_ptr<ClientMessage>> messages;
    
    // Wait for a read event in the client socket
    SocketSelector selector;
    selector.add_wait(SelectedEventType::Read, client_socket);
    auto events = selector.wait(timeout_ms);
    
    // Parse events
    if(events.empty())
        return;
    
    // There will only be one event. Read data
    std::vector<uint8_t> read_buf;
    if(!client_socket->read(read_buf)) {
        // Close socket if read says it should close
        client_socket->close();
        return;
    }
    else if(!read_buf.empty()) {
        // Lock read buffer
        const std::lock_guard<std::mutex> r_lock_guard(r_lock);
        
        // Insert data to read buffer
        r_buffer.insert(read_buf);
    }
}

void Client::send_messages(int timeout_ms) {
    // Abort if no data to read
    if(w_buffer.size() == 0)
        return;
    
    // Lock socket and write buffer
    const std::lock_guard<std::mutex> s_lock_guard(s_lock);
    const std::lock_guard<std::mutex> w_lock_guard(w_lock);
    
    // Merge buffer
    std::vector<uint8_t> bytes;
    w_buffer.get(bytes, w_buffer.size());
    size_t sent = 0;
    
    // Setup timer
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    if(timeout_ms > 0)
        start = std::chrono::high_resolution_clock::now();
    
    // Start sending
    while(sent != bytes.size()) {
        // Send data
        //size_t result = client_socket->write(bytes.begin() + sent, bytes.size() - sent);
        size_t result = client_socket->write(bytes.begin() + sent, bytes.size() - sent);
        
        // Clear sent part of buffer
        w_buffer.erase(result);
        sent += result;
        
        // Stop sending if timeout exceeded
        if(timeout_ms == 0)
            break;
        else if(timeout_ms != -1) {
            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            if(elapsed > timeout_ms)
                break;
        }
    }
    
    return;
}

void Client::add_message(const ClientMessage& message) {
    // Lock write buffer
    const std::lock_guard<std::mutex> w_lock_guard(w_lock);
    
    // Insert to buffer
    w_buffer.insert(message.to_bytes());
}

std::deque<std::unique_ptr<ClientMessage>> Client::get_messages() {
    std::deque<std::unique_ptr<ClientMessage>> messages;
    
    // Lock read buffer
    const std::lock_guard<std::mutex> r_lock_guard(r_lock);
    
    // Check if a message can be built from the current read buffer.
    // Try to build as many messages as possible
    while(true) {
        auto message = ClientMessage::from_buffer(r_buffer);
        
        if(!message)
            break;

        // std::move used to transfer ownership to vector
        messages.push_back(std::move(message));
    }
    
    return messages;
}

