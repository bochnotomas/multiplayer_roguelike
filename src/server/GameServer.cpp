#include "GameServer.hpp"
#include "Enemy.hpp"

Map& GameServer::getLevel(int n) {
    // Generate missing levels
    for(auto depth = levels.size(); depth <= n; depth++) {
        // TODO call the level generator here
        Map newLevel;
        newLevel.create_random_map();
        newLevel.objects.emplace_back(new Object('P', Direction::NORTH, true, {3, 2})); // bogus object (POLEN)
        newLevel.objects.emplace_back(new Object('P', Direction::NORTH, true, {1, 5})); // yet another bogus object
        levels.emplace_back(std::move(newLevel));
    }
    
    return levels[n];
}

void GameServer::doTurn() {
    //std::cerr << "Turn!" << std::endl;
    for(auto l = 0; l < levels.size(); l++) {
        //std::cerr << " * Level " << l << std::endl;
        // Get players in this level and do their action
        std::vector<std::shared_ptr<Player> > levelPlayers;
        for(auto player : players) {
            //std::cerr << "   - Player " << player->name << std::endl;
            if(!player->name.empty() && player->level == l) {
                levelPlayers.emplace_back(player);
                if(player->action) {
                    //std::cerr << "     | Has action" << std::endl;
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
                            //std::cerr << "     | UseItemAction (TODO)" << std::endl;
                            // TODO
                            break;
                        default:
                            //std::cerr << "     | Unknown action" << std::endl;
                            break;
                    }
                    
                    player->action = nullptr;
                }
            }
        }
        
        for(auto object : levels[l].objects) {
            // Generic object update
            object->update();
            
            // Object type-specific update
            if (object->get_type() == ObjectType::ENEMY)
                std::dynamic_pointer_cast<Enemy>(object)->aiTick(levelPlayers, levels[l]);
        }
        
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
                //std::cerr << "Accepted; connected = " << connectedCount << "; actions = " << withActionCount << std::endl;
            }
            else {
                //std::cerr << "Rejected; connected = " << connectedCount << "; actions = " << withActionCount << "; duration = " << std::chrono::duration_cast<std::chrono::seconds>(thisTurnStart - lastTurnTime).count() << std::endl;
            }
        }
        else {
            //std::cerr << "Rejected; connected == 0" << std::endl;
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
