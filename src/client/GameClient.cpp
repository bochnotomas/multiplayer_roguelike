#include "GameClient.hpp"
#include "ClearScreenDrawable.hpp"
#include "Menu.hpp"
#include "../server/Map.h"

enum ClientMenuItem {
    TextItem,
    JoinCancel,
    ActionQuit,
};

void GameClient::netLoop() {
    try {
        while(playing) {
            receiveMessages(250);
            sendMessages(250);
        }
    }
    catch(const SocketException& err) {
        playing = false; // TODO error screen?
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
                                // TODO request map
                                
                                // Setup action menu
                                std::shared_ptr<MenuItem> quitAction(new MenuItem(ClientMenuItem::ActionQuit, "Quit"));
                                std::shared_ptr<Menu> actionMenu(new Menu(renderer->getWidth() - minimapW, renderer->getHeight() - minimapH, minimapW, minimapH));
                                actionMenu->addItem(quitAction);
                                actionMenu->toggleExpand(false);
                                actionMenu->setSplit(10);
            
                                std::lock_guard<std::mutex> rLockGuard(renderer->r_lock);
                                renderer->clear_drawables();
                                renderer->add_drawable(clearDrawable);
                                renderer->add_drawable(actionMenu);
                            }
                        }
                    }
                    break;
            }
        }
        
        // Parse input
        switch(renderer->getch()) {
            case 'w':
            case 'W':
                break; // TODO move
            case 's':
            case 'S':
                break; // TODO move
            case 'a':
            case 'A':
                break; // TODO move
            case 'd':
            case 'D':
                break; // TODO move
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
