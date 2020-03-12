#include "ClearScreenDrawable.hpp"
#include "GameClient.hpp"
#include "Menu.hpp"
#include "Camera.h"
#include "../server/Map.h"
#include "PlayerSnapshot.hpp"

enum ClientMenuItem {
    TextItem,
    JoinCancel,
    ActionQuit,
};

void GameClient::netLoop() {
    try {
        while(playing) {
            // Receive messages with 250 ms timeout
            receiveMessages(250);
            
            // Send messages with 250 ms timeout
            sendMessages(250);
            
            // Kill network loop if socket was closed
            if(!isSocketOpen())
                playing = false;
        }
    }
    catch(const SocketException& err) {
        playing = false; // TODO error screen?
        return;
    }
}

void GameClient::logic(Renderer* renderer) {
    const auto minimapW = 20;
    const auto minimapH = 10;
    const auto midX = renderer->getWidth() / 2;
    const auto midY = renderer->getHeight() / 2;
    std::shared_ptr<ClearScreenDrawable> clearDrawable(new ClearScreenDrawable());
    
    // Join with a bogus name TODO change name with menu (needs InputMenuItem)
    std::string playerName = "test_user_" + std::to_string(time(NULL));
    addMessage(ClientMessageDoJoin(playerName));
    
    // Setup renderer with joining message
    std::shared_ptr<Map> map = nullptr;
    std::shared_ptr<Camera> cam = nullptr;
    std::shared_ptr<Menu> focus = nullptr;
    {
        std::shared_ptr<MenuItem> joiningText(new MenuItem(ClientMenuItem::TextItem, "Joining as " + playerName + "..."));
        std::shared_ptr<MenuItem> joiningCancel(new MenuItem(ClientMenuItem::JoinCancel, "Cancel"));
        std::shared_ptr<Menu> joiningMenu(new Menu(4, 3, midX, midY));
        joiningMenu->addItem(joiningText);
        joiningMenu->addItem(joiningCancel);
        joiningMenu->toggleCenter(true);
        
        std::lock_guard<std::mutex> rLockGuard(renderer->r_lock);
        renderer->clear_drawables();
        renderer->add_drawable(clearDrawable);
        renderer->add_drawable(joiningMenu);
        focus = joiningMenu;
    }
    
    // Player data this turn
    std::vector<PlayerSnapshot> players;
    
    // Client logic loop
    bool joined = false;
    while(playing) {
        // Parse messages
        auto messages = getMessages();
        for(auto it = messages.begin(); it != messages.end(); it++) {
            switch((*it)->type) {
                case GameMessageType::Join:
                    {
                        auto joinEvent = dynamic_cast<ClientMessageJoin*>(it->get());
                        if(joined) {
                            // TODO popup?
                        }
                        else {
                            if(joinEvent->senderName == playerName) {
                                joined = true;
                                focus = nullptr;
                                
                                // Setup action menu
                                std::shared_ptr<MenuItem> quitAction(new MenuItem(ClientMenuItem::ActionQuit, "Quit"));
                                std::shared_ptr<Menu> actionMenu(new Menu(renderer->getWidth() - minimapW, 4, minimapW, renderer->getHeight() - 4));
                                actionMenu->addItem(quitAction);
                                actionMenu->toggleExpand(false);
                                actionMenu->setSplit(10);
            
                                std::lock_guard<std::mutex> rLockGuard(renderer->r_lock);
                                renderer->clear_drawables();
                                renderer->add_drawable(clearDrawable);
                                if(map) {
                                    cam = std::shared_ptr<Camera>(new Camera(' ', map.get(), { 5, 5 }, { 20, 10 }));
                                    renderer->add_drawable(cam);
                                }
                                renderer->add_drawable(actionMenu);
                                focus = actionMenu;
                            }
                        }
                    }
                    break;
                case GameMessageType::MapTileData:
                    {
                        auto mapTileDataMessage = dynamic_cast<ClientMessageMapTileData*>(it->get());
                        if(!map)
                            map = std::shared_ptr<Map>(new Map());
                        map->generate_square_map(mapTileDataMessage->width, mapTileDataMessage->height); // TODO better api
                        *map->get_map_plane() = mapTileDataMessage->tileData;
                    }
                    break;
                case GameMessageType::MapObjectData:
                    {
                        auto mapObjectDataMessage = dynamic_cast<ClientMessageMapObjectData*>(it->get());
                        // Throw this message in the garbage can if there isn't
                        // any map tile data yet
                        if(!map)
                            break;
                        // TODO put objects in map _I NEED SHARED PTR VECTOR INSTEAD FOR THIS TO WORK IN MAP::OBJECTS_
                    }
                    break;
                case GameMessageType::PlayerData:
                    {
                        auto playerDataMessage = dynamic_cast<ClientMessagePlayerData*>(it->get());
                        players.clear();
                        for(auto i = 0; i < playerDataMessage->names.size(); i++) {
                            players.emplace_back(
                                playerDataMessage->names[i],
                                playerDataMessage->positions[i].first,
                                playerDataMessage->positions[i].second,
                                playerDataMessage->levels[i]
                            );
                        }
                    }
                    break;
            }
        }
        
        // Parse input
        if(renderer->kbhit()) {
            switch(renderer->getch()) {
                case 'w':
                case 'W':
                    if(cam)
                        cam->move(Direction::NORTH);
                    break;
                case 's':
                case 'S':
                    if(cam)
                        cam->move(Direction::SOUTH);
                    break;
                case 'a':
                case 'A':
                    if(cam)
                        cam->rotate(-0.1f);
                    break;
                case 'd':
                case 'D':
                    if(cam)
                        cam->rotate(0.1f);
                    break;
                case 'r':
                case 'R':
                    if(focus)
                        focus->moveCursor(-1);
                    break;
                case 'f':
                case 'F':
                    if(focus)
                        focus->moveCursor(1);
                    break;
                case ' ':
                case '\n':
                    if(focus) {
                        auto selection = focus->selectCursor();
                        if(!selection)
                            break;
                        
                        switch(selection->getKey()) {
                            case ClientMenuItem::JoinCancel:
                            case ClientMenuItem::ActionQuit:
                                addMessage(ClientMessageDoQuit());
                                playing = false;
                                break;
                        }
                    }
                    break;
            }
        }
        else // Wait if there is no input. Logic should run at most at 60 FPS
            std::this_thread::sleep_for(std::chrono::microseconds(16667));
    }
    
    renderer->clear_drawables_lock();
}

GameClient::GameClient(Renderer* renderer, std::string host, uint16_t port) :
    Client(host, port, 5000), // 5s timeout
    renderer(renderer),
    playing(true)
{
    // Start network thread
    netThread = std::thread(&GameClient::netLoop, this);
    
    // Start the client logic
    logic(renderer);
    
    // Wait for network thread to die
    netThread.join();
    
    // Clear drawables
    renderer->clear_drawables_lock();
}
