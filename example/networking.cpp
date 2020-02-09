#include "../src/server/server.hpp"
#include "../src/client/client.hpp"
#include <iostream>
#include <limits>

void start_server(unsigned short port) {
    std::cout << "Starting server with port " << port << "..." << std::endl;
    auto server = Server(port);
    std::cout << "Server started" << std::endl;
    
    bool running = true;
    while(running) {
        // Get messages
        std::deque<std::shared_ptr<ServerMessage>> messages = server.receive(250);
        
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
                        auto join_event = std::dynamic_pointer_cast<ServerMessageDoJoin>(*it);
                        if(join_event->sender->name.empty()) {
                            std::cout << prefix << " joined as '" << join_event->name << '\'' << std::endl;
                            join_event->sender->name = join_event->name;
                        }
                        else
                            std::cout << prefix << " tried to join as '" << join_event->name << "', but this was refused as they already joined" << std::endl;
                    }
                    break;
                case GameMessageType::DoQuit:
                    std::cout << prefix << " quit" << std::endl;
                    break;
                case GameMessageType::DoChat:
                    {
                        auto char_event = std::dynamic_pointer_cast<ServerMessageDoChat>(*it);
                        std::cout << prefix << ": " << char_event->message << std::endl;
                        if(char_event->message == "@kill_server")
                            running = false;
                    }
                    break;
                default:
                    std::cout << "Ignored unexpected message of type " << (*it)->type << std::endl;
                    continue;
            }
            
            std::unique_ptr<ClientMessage> client_message = (*it)->to_client();
            if(client_message)
                server.add_message_all_except(*client_message, (*it)->sender);
        }
        
        // Send buffered messages
        server.send_messages(250);
    }
}

void start_client(std::string host, unsigned short port) {
    std::cout << "Connecting to " << host << ':' << port << "..." << std::endl;
    auto client = Client(host, port, 5000);
    std::cout << "Connected. Type '!join' to join, '!quit' to quit or say '@kill_server' to kill the server" << std::endl;
    
    bool running = true;
    std::string name;
    while(running) {
        // Get messages
        std::deque<std::shared_ptr<ClientMessage>> messages = client.receive(500);
        
        // Parse messages
        for(auto it = messages.begin(); it != messages.end(); it++) {
            
            std::string prefix;
            if((*it)->sender_name.empty())
                prefix = "~anon~";
            else
                prefix = "<" + (*it)->sender_name + ">";
            
            switch((*it)->type) {
                case GameMessageType::Join:
                    std::cout << prefix << " joined" << std::endl;
                    break;
                case GameMessageType::Quit:
                    std::cout << prefix << " quit" << std::endl;
                    break;
                case GameMessageType::Chat:
                    {
                        auto char_event = std::dynamic_pointer_cast<ClientMessageChat>(*it);
                        std::cout << prefix << ": " << char_event->message << std::endl;
                    }
                    break;
                default:
                    std::cout << "Ignored unexpected message of type " << (*it)->type << std::endl;
            }
        }
        
        // Send a chat message
        std::string message;
        std::cout << name << "> " << std::flush;
        std::getline(std::cin, message);
        
        if(message == "!join") {
            if(!name.empty()) {
                std::cout << "Already joined as '" << name << '\'' << std::endl;
            }
            
            std::cout << "What do you want to be called?\n> " << std::flush;
            std::getline(std::cin, name);
            client.add_message(ClientMessageDoJoin(name));
        }
        else if(message == "!quit")
            running = false;
        else
            client.add_message(ClientMessageDoChat(message));
        
        // Send all messages with a timeout of 2 seconds
        client.send_messages(2000); // TODO warning? retry?
    }
}

int main(int argc, char **argv) {
    std::cout << "Enter port number: ";
    unsigned short port;
    std::cin >> port;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Enter host (leave empty to host): ";
    std::string host;
    std::getline(std::cin, host);
    
    try {
        if(host.empty())
            start_server(port);
        else
            start_client(host, port);
    }
    catch(SocketException e) {
        std::cout << "Network exception: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Stopped with no exceptions. Goodbye..." << std::endl;
    return 0;
}

