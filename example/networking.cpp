#include "../src/server/Server.hpp"
#include "../src/client/Client.hpp"
#include <iostream>
#include <limits>
#include <thread>
#include <atomic>

void startServer(unsigned short port) {
    std::cout << "Starting server with port " << port << "..." << std::endl;
    auto server = Server(port);
    std::cout << "Server started" << std::endl;
    
    bool running = true;
    while(running) {
        // Get messages
        std::deque<std::shared_ptr<ServerMessage> > messages = server.receive(250);
        
        // Sleep if there are no players connected
        if(server.players.empty())
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        
        // Parse messages
        for(auto it = messages.begin(); it != messages.end(); it++) {
            std::string prefix;
            if((*it)->sender->name.empty())
                prefix = "~anon~";
            else
                prefix = "<" + (*it)->sender->name+ ">";
            
            switch((*it)->type) {
                case GameMessageType::DoJoin:
                    {
                        auto joinEvent = std::dynamic_pointer_cast<ServerMessageDoJoin>(*it);
                        if(joinEvent->sender->name.empty()) {
                            std::cout << prefix << " joined as '" << joinEvent->name << '\'' << std::endl;
                            joinEvent->sender->name = joinEvent->name;
                        }
                        else
                            std::cout << prefix << " tried to join as '" << joinEvent->name << "', but this was refused as they already joined" << std::endl;
                    }
                    break;
                case GameMessageType::DoQuit:
                    std::cout << prefix << " quit" << std::endl;
                    break;
                case GameMessageType::DoChat:
                    {
                        auto charEvent = std::dynamic_pointer_cast<ServerMessageDoChat>(*it);
                        std::cout << prefix << ": " << charEvent->message << std::endl;
                        if(charEvent->message == "@kill_server")
                            running = false;
                    }
                    break;
                default:
                    std::cout << "Ignored unexpected message of type " << (*it)->type << std::endl;
            }
            
            std::unique_ptr<ClientMessage> clientMessage = (*it)->toClient();
            if(clientMessage)
                server.addMessageAllExcept(*clientMessage, (*it)->sender);
        }
        
        // Send buffered messages
        server.sendMessages(250);
    }
}

void clientThreadLoop(std::shared_ptr<Client> client, std::shared_ptr<std::atomic<bool> > running) {
    try {
        while(*running) {
            // Receive data
            client->receiveMessages(250);

            // Parse messages
            auto messages = client->getMessages();
            for(auto it = messages.begin(); it != messages.end(); it++) {
                std::string prefix;
                if((*it)->senderName.empty())
                    prefix = "~anon~";
                else
                    prefix = "<" + (*it)->senderName + ">";

                switch((*it)->type) {
                    case GameMessageType::Join:
                        std::cout << prefix << " joined" << std::endl;
                        break;
                    case GameMessageType::Quit:
                        std::cout << prefix << " quit" << std::endl;
                        break;
                    case GameMessageType::Chat:
                        {
                            auto charEvent = dynamic_cast<ClientMessageChat*>(it->get());
                            std::cout << prefix << ": " << charEvent->message << std::endl;
                        }
                        break;
                    default:
                        std::cout << "Ignored unexpected message of type " << (*it)->type << std::endl;
                }
            }
        
            // Send data
            client->sendMessages(250);
        }
    }
    catch(SocketException e) {
        std::cout << "Network exception in network thread: " << e.what() << std::endl;
    }

    *running = false;
}

void startClient(std::string host, unsigned short port) {
    std::cout << "Connecting to " << host << ':' << port << "..." << std::endl;
    auto client = std::shared_ptr<Client>(new Client(host, port, 5000));
    auto running = std::shared_ptr<std::atomic<bool>>(new std::atomic<bool>(true));
    std::thread networkThread(clientThreadLoop, client, running);
    std::cout << "Connected. Type '!join' to join, '!quit' to quit or say '@kill_server' to kill the server" << std::endl;
    
    std::string name;
    while(*running) {
        // Send a chat message
        std::string message;
        std::cout << name << "> " << std::flush;
        std::getline(std::cin, message);
        
        if(message == "!join") {
            if(!name.empty()) {
                std::cout << "Already joined as '" << name << '\'' << std::endl;
                continue;
            }
            
            std::cout << "What do you want to be called?\n> " << std::flush;
            std::getline(std::cin, name);
            client->addMessage(ClientMessageDoJoin(name));
        }
        else if(message == "!quit")
            *running = false;
        else
            client->addMessage(ClientMessageDoChat(message));
    }

    networkThread.join();
}

int main(int argc, char **argv) {
    Socket::initSocketApi();
    
    std::cout << "Enter port number: ";
    unsigned short port;
    std::cin >> port;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Enter host (leave empty to host): ";
    std::string host;
    std::getline(std::cin, host);
    
    try {
        if(host.empty())
            startServer(port);
        else
            startClient(host, port);
    }
    catch(SocketException e) {
        std::cout << "Network exception: " << e.what() << std::endl;
    }
    
    std::cout << "Goodbye..." << std::endl;
    Socket::cleanupSocketApi();
    return 0;
}

