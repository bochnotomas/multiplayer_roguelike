#include "GameServer.hpp"
#include "Enemy.hpp"

Map& GameServer::getLevel(int n) {
    // Generate missing levels
    for(auto depth = levels.size(); depth <= n; depth++) {
        // TODO call the level generator here
        Map newLevel;
        //newLevel.create_random_map();
        newLevel.set_preset_map();
        levels.emplace_back(std::move(newLevel));
    }
    
    return levels[n];
}

void GameServer::doTurn() {
    for(auto l = 0; l < levels.size(); l++) {
        // Get players in this level
        std::vector<std::shared_ptr<Player> > levelPlayers;
        for(auto player : players) {
            if(player->level == l)
                levelPlayers.emplace_back(player);
        }
        
        for(auto object : levels[l].objects) {
            // Generic object update
            object->update();
            
            // Object type-specific update
            if (object->get_type() == ObjectType::ENEMY)
                std::dynamic_pointer_cast<Enemy>(object)->aiTick(levelPlayers, levels[l]);
        }
        
        ClientMessageMapObjectData objectUpdateMessage(levels[l].objects);
        for(auto player : players) {
            if(player->level == l)
                addMessage(objectUpdateMessage, player);
        }
    }
    
    addMessageAll(ClientMessagePlayerData(players));
}

void GameServer::logic() {
    int killCounter = 0;
    while(running) {
        // Get messages
        std::deque<std::shared_ptr<ServerMessage> > messages = receive(250);
        
        // Kill server if there are no players connected for 10 loops, waiting
        // on every iteration for 250ms
        if(players.empty()) {
            if(++killCounter >= 10) {
                running = false;
                break;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        else
            killCounter = 0;
        
        // Parse messages
        for(auto it = messages.begin(); it != messages.end(); it++) {
            bool propagate = false;
            bool propagateAll = false;
            switch((*it)->type) {
                case GameMessageType::DoJoin:
                    {
                        auto joinEvent = std::dynamic_pointer_cast<ServerMessageDoJoin>(*it);
                        // Players with no name haven't joined yet
                        if(joinEvent->sender->name.empty() && !joinEvent->name.empty()) {
                            joinEvent->sender->name = joinEvent->name;
                            propagate = true;
                            propagateAll = true;
                        }
                        
                        // Send level data and player data to newly joined player
                        auto thisLevel = getLevel(0);
                        addMessage(ClientMessageMapTileData(thisLevel), joinEvent->sender);
                        addMessage(ClientMessageMapObjectData(thisLevel.objects), joinEvent->sender);
                        addMessage(ClientMessagePlayerData(players), joinEvent->sender);
                    }
                    break;
                case GameMessageType::DoQuit:
                    propagate = true;
                    break;
                // TODO parse message types
            }
            
            if(propagate) {
                auto clientMessage = (*it)->toClient();
                if(!clientMessage)
                    continue;
                
                if(propagateAll)
                    addMessageAll(*clientMessage);
                else
                    addMessageAllExcept(*clientMessage, (*it)->sender);
            }
        }
        
        // Simulate turn
        // TODO only do this when all players sent their actions
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        doTurn();
        
        // Send buffered messages
        sendMessages(250);
        
        // Kill server if the socket is closed
        if(!isSocketOpen()) {
            running = false;
            return;
        }
    }
    
    if(isSocketOpen())
        close();
}

GameServer::GameServer(uint16_t port) :
    Server(port),
    running(false)
{}

GameServer::~GameServer() {
    stop();
}

void GameServer::start() {
    if(running)
        throw std::runtime_error("GameServer::start: Attempt to start server when it is already running");
    
    thread = std::thread(&GameServer::logic, this);
    running = true;
}

void GameServer::stop() {
    if(running)
        running = false;
    
    if(thread.joinable())
        thread.join();
}
