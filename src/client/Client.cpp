#include "../networking/SocketSelector.hpp"
#include "Client.hpp"
#include <chrono>

Client::Client(std::string host, uint16_t port, int timeoutMs) :
    // Open connection socket
    clientSocket(new Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
{
    // Resolve host
    auto addresses = Socket::resolve(host);
    
    // Connect to first available socket
    SocketSelector selector;
    for(auto it = addresses.begin(); it != addresses.end(); it++) {
        try {
            // Create socket
            std::shared_ptr<Socket> candidateSocket(new Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
            
            // Connect
            candidateSocket->connect(AF_INET, *it, port);
            
            // Add to selector. Wait for writing to become available
            selector.addWait(SelectedEventType::Write, candidateSocket);
        }
        catch(SocketException){}; // Ignore exceptions, just don't connect
    }
    
    // Wait for any connection to be accepted
    auto events = selector.wait(timeoutMs);
    
    // Parse events, accepting first available connection. All other sockets
    // are automatically closed
    std::shared_ptr<Socket> chosenCandidate = nullptr;
    for(auto it = events.begin(); it != events.end(); it++) {
        // Pick first available connection
        if(it->isOfType(SelectedEventType::Write) && it->socket->isValid()) {
            chosenCandidate = std::move(it->socket);
            break;
        }
    }
    
    if(chosenCandidate == nullptr)
        throw SocketException("Failed to connect to server: timed out");
    
    // Close connection socket automatically and replace with new connection
    clientSocket = std::move(chosenCandidate);
    
    // Make socket non-blocking as we will be reading and writing from now on
    clientSocket->setBlocking(false);
}

Client::~Client() {
    if(clientSocket->isValid()) {
        try {
            // Shutdown socket read
            clientSocket->shutdown(SocketShutdownMode::ShutRead);
        
            // Send all buffered data. Abort after, at most, 10 seconds
            auto quarters = 0;
            while(wBuffer.size() > 0) {
                sendMessages(250);
                if(++quarters >= 40)
                    break;
            }
            
            // Shutdown socket completely
            clientSocket->shutdown(SocketShutdownMode::ShutReadWrite);
        }
        catch(SocketException e) {}; // Ignore network exceptions
    }
}

void Client::receiveMessages(int timeoutMs) {
    // Lock socket
    const std::lock_guard<std::mutex> sLockGuard(sLock);
        
    std::deque<std::shared_ptr<ClientMessage>> messages;
    
    // Wait for a read event in the client socket
    SocketSelector selector;
    selector.addWait(SelectedEventType::Read, clientSocket);
    auto events = selector.wait(timeoutMs);
    
    // Parse events
    if(events.empty())
        return;
    
    // There will only be one event. Read data
    std::vector<uint8_t> readBuf;
    if(!clientSocket->read(readBuf)) {
        // Close socket if read says it should close
        clientSocket->close();
        return;
    }
    else if(!readBuf.empty()) {
        // Lock read buffer
        const std::lock_guard<std::mutex> rLockGuard(rLock);
        
        // Insert data to read buffer
        rBuffer.insert(readBuf);
    }
}

void Client::sendMessages(int timeoutMs) {
    // Abort if no data to read
    if(wBuffer.size() == 0)
        return;
    
    // Lock socket and write buffer
    const std::lock_guard<std::mutex> sLockGuard(sLock);
    const std::lock_guard<std::mutex> wLockGuard(wLock);
    
    // Merge buffer
    std::vector<uint8_t> bytes;
    wBuffer.get(bytes, wBuffer.size());
    size_t sent = 0;
    
    // Setup timer
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    if(timeoutMs > 0)
        start = std::chrono::high_resolution_clock::now();
    
    // Start sending
    while(sent != bytes.size()) {
        // Send data
        //size_t result = client_socket->write(bytes.begin() + sent, bytes.size() - sent);
        size_t result = clientSocket->write(bytes.begin() + sent, bytes.size() - sent);
        
        // Clear sent part of buffer
        wBuffer.erase(result);
        sent += result;
        
        // Stop sending if timeout exceeded
        if(timeoutMs == 0)
            break;
        else if(timeoutMs != -1) {
            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            if(elapsed > timeoutMs)
                break;
        }
    }
    
    return;
}

void Client::addMessage(const ClientMessage& message) {
    // Lock write buffer
    const std::lock_guard<std::mutex> wLockGuard(wLock);
    
    // Insert to buffer
    wBuffer.insert(message.toBytes());
}

std::deque<std::unique_ptr<ClientMessage>> Client::getMessages() {
    std::deque<std::unique_ptr<ClientMessage>> messages;
    
    // Lock read buffer
    const std::lock_guard<std::mutex> rLockGuard(rLock);
    
    // Check if a message can be built from the current read buffer.
    // Try to build as many messages as possible
    while(true) {
        auto message = ClientMessage::fromBuffer(rBuffer);
        
        if(!message)
            break;

        // std::move used to transfer ownership to vector
        messages.push_back(std::move(message));
    }
    
    return messages;
}

bool Client::isSocketOpen() {
    return clientSocket->isValid();
}

