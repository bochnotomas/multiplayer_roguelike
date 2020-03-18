#include "ClearScreenDrawable.hpp"
#include "GameClient.hpp"
#include "Menu.hpp"
#include "Camera.h"
#include "InputMenuItem.hpp"
#include "../server/Map.h"

enum ClientMenuItem {
    TextItem,
    JoinCancel,
    ActionQuit,
    NameOk,
    NameCancel,
    InventoryItem
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
    
    // Setup renderer with name menu
    std::string playerName;
    std::shared_ptr<Map> map = nullptr;
    std::shared_ptr<Camera> cam = nullptr;
    std::shared_ptr<Menu> focus = nullptr;
    
    std::shared_ptr<MenuItem> quitAction(new MenuItem(ClientMenuItem::ActionQuit, "Quit"));
    std::shared_ptr<Menu> actionMenu(new Menu(renderer->getWidth() - minimapW, 4, minimapW, renderer->getHeight() - 4));
    actionMenu->addItem(quitAction);
    actionMenu->toggleExpand(false);
    actionMenu->setSplit(10);
                            
    std::shared_ptr<InputMenuItem> nameInput(new InputMenuItem(ClientMenuItem::TextItem, "My name is", "Riley"));
    {
        std::shared_ptr<MenuItem> nameText(new MenuItem(ClientMenuItem::TextItem, "What is your name?", false));
        std::shared_ptr<MenuItem> nameBlank(new MenuItem(ClientMenuItem::TextItem, "", false));
        std::shared_ptr<MenuItem> nameOk(new MenuItem(ClientMenuItem::NameOk, "Start playing"));
        std::shared_ptr<MenuItem> nameCancel(new MenuItem(ClientMenuItem::NameCancel, "Cancel"));
        std::shared_ptr<Menu> nameMenu(new Menu(4, 3, midX, midY));
        nameMenu->addItem(nameText);
        nameMenu->addItem(nameInput);
        nameMenu->addItem(nameBlank);
        nameMenu->addItem(nameOk);
        nameMenu->addItem(nameCancel);
        nameMenu->toggleCenter(true);
        
        std::lock_guard<std::mutex> rLockGuard(renderer->r_lock);
        renderer->clear_drawables();
        renderer->add_drawable(clearDrawable);
        renderer->add_drawable(nameMenu);
        focus = nameMenu;
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
                                std::lock_guard<std::mutex> rLockGuard(renderer->r_lock);
                                renderer->clear_drawables();
                                renderer->add_drawable(clearDrawable);
                                if(map) {
                                    cam = std::shared_ptr<Camera>(new Camera(' ', map.get(), { 0, 0 }, { 20, 10 }));
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
                        if(!map)
                            map = std::shared_ptr<Map>(new Map());
                        map->objects = mapObjectDataMessage->objects;
                    }
                    break;
                case GameMessageType::PlayerData:
                    {
                        auto playerDataMessage = dynamic_cast<ClientMessagePlayerData*>(it->get());
                        players = playerDataMessage->playersSnapshots;
                        for(auto player : players) {
                            if(player.name == playerName) {
                                if(cam)
                                    cam->set_position({player.x, player.y});
                                
                                std::lock_guard<std::mutex> rLockGuard(renderer->r_lock);
                                actionMenu->clearItems();
                                actionMenu->addItem(quitAction);
                                for(auto item : player.items) {
                                    std::shared_ptr<MenuItem> inventoryItem(new MenuItem(ClientMenuItem::InventoryItem, item));
                                    actionMenu->addItem(inventoryItem);
                                }
                                
                                if(player.items.empty())
                                    actionMenu->setCursor(0);
                                else
                                    actionMenu->setCursor(1);
                                break;
                            }
                        }
                    }
                    break;
            }
        }
        
        // Parse input
        if(renderer->kbhit()) {
            char input = renderer->getch();
            if(!focus || !focus->input(input)) {
                switch(input) {
                    case 'w':
                    case 'W':
                        if(!joined) {
                            if(focus)
                                focus->moveCursor(-1);
                        }
                        else if(cam)
                            addMessage(ClientMessageDoAction(MoveAction(cam->getMapDirection(Direction::NORTH))));
                        break;
                    case 's':
                    case 'S':
                        if(!joined) {
                            if(focus)
                                focus->moveCursor(1);
                        }
                        else if(cam)
                            addMessage(ClientMessageDoAction(MoveAction(cam->getMapDirection(Direction::SOUTH))));
                        break;
                    case 'a':
                    case 'A':
                        if(joined && cam)
                            cam->rotate(-0.1f);
                        break;
                    case 'd':
                    case 'D':
                        if(joined && cam)
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
                    case '\r':
                        if(focus) {
                            auto selection = focus->selectCursor();
                            if(!selection)
                                break;
                            
                            switch(selection->getKey()) {
                                case ClientMenuItem::NameCancel:
                                case ClientMenuItem::JoinCancel:
                                case ClientMenuItem::ActionQuit:
                                    addMessage(ClientMessageDoQuit());
                                    playing = false;
                                    break;
                                case ClientMenuItem::NameOk:
                                    {
                                        auto inputName = nameInput->get();
                                        if(inputName.empty())
                                            break;
                                        
                                        playerName = inputName;
                                        addMessage(ClientMessageDoJoin(playerName));
                                        std::shared_ptr<MenuItem> joiningText(new MenuItem(ClientMenuItem::TextItem, "Joining as " + playerName + "...", false));
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
                                    break;
                            }
                        }
                        break;
                }
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
