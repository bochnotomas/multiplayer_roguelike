#include "../src/server/server.hpp"
#include "../src/client/client.hpp"
#include <iostream>

void start_server(unsigned short port) {
    std::cout << "Starting server with port " << port << "..." << std::endl;
    auto server = Server(port);
    std::cout << "Server started" << std::endl;
    
    bool running = true;
    while(running) {
        // Get events
        std::deque<std::shared_ptr<GameEvent>> game_events = server.receive(500);
        
        // Parse events
        for(auto it = game_events.begin(); it != game_events.end(); it++) {
            
            std::string prefix;
            if((*it)->player.empty())
                prefix = "~anon~";
            else
                prefix = "<" + (*it)->player + ">";
            
            switch((*it)->type) {
                case GameEventType::Join:
                    std::cout << prefix << " joined" << std::endl;
                    server.add_event_all_except(
                        GameEventJoin((*it)->player),
                        (*it)->player
                    );
                    
                    break;
                case GameEventType::DoQuit:
                    std::cout << prefix << " quit" << std::endl;
                    server.add_event_all_except(
                        GameEventQuit((*it)->player),
                        (*it)->player
                    );
                    
                    break;
                case GameEventType::DoChat:
                    {
                        auto char_event = std::dynamic_pointer_cast<GameEventDoChat>(*it);
                        std::cout << prefix << ": " << char_event->message << std::endl;
                        if(char_event->message == "@kill_server")
                            running = false;
                        else {
                            server.add_event_all_except(
                                GameEventChat(char_event->player, char_event->message),
                                char_event->player
                            );
                        }
                    }
                    break;
                default:
                    std::cout << "Ignored non-action event" << std::endl;
            }
        }
        
        // Send buffered events
        server.send_events(500);
    }
}

void start_client(std::string host, unsigned short port) {
    std::cout << "Connecting to " << host << ':' << port << "..." << std::endl;
    auto client = Client(host, port, 5000);
    std::cout << "Connected. Type '!join' to join, '!quit' to quit or say '@kill_server' to kill the server" << std::endl;
    
    bool running = true;
    std::string name;
    while(running) {
        // Get events
        std::deque<std::shared_ptr<GameEvent>> game_events = client.receive(500);
        
        // Parse events
        for(auto it = game_events.begin(); it != game_events.end(); it++) {
            
            std::string prefix;
            if((*it)->player.empty())
                prefix = "~anon~";
            else
                prefix = "<" + (*it)->player + ">";
            
            switch((*it)->type) {
                case GameEventType::Join:
                    std::cout << prefix << " joined" << std::endl;
                    break;
                case GameEventType::Quit:
                    std::cout << prefix << " quit" << std::endl;
                    break;
                case GameEventType::Chat:
                    {
                        auto char_event = std::dynamic_pointer_cast<GameEventChat>(*it);
                        std::cout << prefix << ": " << char_event->message << std::endl;
                    }
                    break;
                default:
                    std::cout << "Ignored action event" << std::endl;
            }
        }
        
        // Send a message
        std::string message;
        std::cout << name << "> " << std::flush;
        std::getline(std::cin, message);
        
        if(message == "!join") {
            if(!name.empty()) {
                std::cout << "Already joined as '" << name << '\'' << std::endl;
            }
            
            std::cout << "What do you want to be called?\n> " << std::flush;
            std::getline(std::cin, name);
            client.add_event(GameEventJoin(name));
        }
        else if(message == "!quit")
            running = false;
        else
            client.add_event(GameEventDoChat(message));
        
        // Send all events with a timeout of 2 seconds
        client.send_events(2000); // TODO warning? retry?
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

