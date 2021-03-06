#include "LevelGeneration2D.h"
#include "GameServer.hpp"
#include "Enemy.hpp"
#include "WeaponSword.h"

Map& GameServer::getLevel(int n) {
    // Generate missing levels
    for(auto depth = levels.size(); depth <= n; depth++) {
        LevelGeneration2D generator;
        Map newLevel = generator.create_random_map();
        levels.emplace_back(std::move(newLevel));
    }
    
    return levels[n];
}

void GameServer::doTurn() {
    for(auto l = 0; l < levels.size(); l++) {
        // Get players in this level and do their action
        std::vector<std::shared_ptr<Player> > levelPlayers;
        for(auto player : players) {
            if(!player->name.empty() && player->level == l) {
                levelPlayers.emplace_back(player);
                if(player->action) {
                    switch(player->action->type) {
                        case ActionType::Move:
                            {
                                const auto& moveAction = static_cast<MoveAction*>(player->action.get());
                                //std::cerr << "     | MoveAction with direction " << (int)moveAction->getDirection() << std::endl;
                                auto dir = moveAction->getDirection();
                                if(dir != eDirection::INVALID && dir != eDirection::STOP) {
                                    player->dir = dir;
                                    player->playerMovementLogic(levels[l]);
                                }
                            }
                            break;
                        case ActionType::UseItem:
                            {
                                const auto& useItemAction = static_cast<UseItemAction*>(player->action.get());
                                auto itemPos = useItemAction->getItem();
                                if(itemPos < 0 || itemPos >= player->inventory.inventory.size())
                                    break;
                                
                                // TODO drink or something -> nick's job
                                const auto& item = player->inventory.inventory[itemPos];
                                if(item.itemType == "SWORD_WEAPON")
                                    player->playerAttack(levels[l]);
                            }
                            break;
                    }
                    
                    player->action = nullptr;
                }
            }
        }
        
        for(auto it = levels[l].objects.begin(); it != levels[l].objects.end();) {
            // Remove if object is a player
            if((*it)->get_type() == ObjectType::PLAYER) {
                it = levels[l].objects.erase(it);
                continue;
            }
            
            // Generic object update
            (*it)->update();
            
            // Object type-specific update
            if ((*it)->get_type() == ObjectType::ENEMY)
                std::dynamic_pointer_cast<Enemy>(*it)->aiTick(levelPlayers, levels[l]);
            
            it++;
        }
        
        // Add players to level
        for(auto player : levelPlayers)
            levels[l].objects.push_back(player);
        
        ClientMessageMapTileData tileDataMessage(levels[l]);
        ClientMessageMapObjectData objectUpdateMessage(levels[l].objects);
        for(auto player : levelPlayers) {
            if(player->level != l)
                addMessage(tileDataMessage, player);
            addMessage(objectUpdateMessage, player);
        }
    }
    
    addMessageAll(ClientMessagePlayerData(players));
}

void GameServer::logic() {
    int killCounter = 0;
    auto lastTurnTime = std::chrono::high_resolution_clock::now();
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
                            // Set initial position
                            // TODO pick a place with no wall
                            joinEvent->sender->set_position({2, 2});
                            propagate = true;
                            propagateAll = true;
                            
                            // Give player a sword
                            joinEvent->sender->inventory.inventory.push_back(WeaponSword({0,0}));
                            
                            // Send level data and player data to newly joined player
                            auto thisLevel = getLevel(0);
                            addMessage(ClientMessageMapTileData(thisLevel), joinEvent->sender);
                            addMessage(ClientMessageMapObjectData(thisLevel.objects), joinEvent->sender);
                            addMessage(ClientMessagePlayerData(players), joinEvent->sender);
                        }
                    }
                    break;
                case GameMessageType::DoQuit:
                    propagate = true;
                    break;
                case GameMessageType::DoAction:
                    {
                        auto doActionEvent = std::dynamic_pointer_cast<ServerMessageDoAction>(*it);
                        // Ignore when the player hasn't joined
                        if(doActionEvent->sender->name.empty())
                            break;
                        
                        // TODO check if action can be done
                        
                        // Set player action
                        doActionEvent->sender->action = std::unique_ptr<Action>(new Action(std::move(doActionEvent->action)));
                        addMessage(ClientMessageActionAck(true), doActionEvent->sender);
                    }
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
        
        // Simulate turn if all actions done
        size_t connectedCount = 0;
        size_t withActionCount = 0;
        for(auto& player : players) {
            if(!player->name.empty()) {
                connectedCount++;
                if(player->action)
                    withActionCount++;
            }
        }
        
        auto thisTurnStart = std::chrono::high_resolution_clock::now();
        if(connectedCount > 0) {
            // Do turn if everyone sent their action or more than 10 seconds
            // have passed
            if(connectedCount == withActionCount || std::chrono::duration_cast<std::chrono::seconds>(thisTurnStart - lastTurnTime).count() >= 10) {
                lastTurnTime = thisTurnStart;
                doTurn();
            }
        }
        else {
            lastTurnTime = thisTurnStart;
        }
        
        // Send buffered messages
        try {
            sendMessages(250);
        }
        catch(SocketException e) {}; // Ignore socket exceptions, broken pipe
        
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
