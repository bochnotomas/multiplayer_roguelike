#include "client/Menu.hpp"
#include "server/LevelGeneration2D.h"
#include "client/GameClient.hpp"
#include "server/GameServer.hpp"
#include "client/ClearScreenDrawable.hpp"

// num of columns rendered on the screen
constexpr unsigned short RENDER_WIDTH = 120;
// num of rows rendered on the screen
constexpr unsigned short RENDER_HEIGHT = 30;

enum MenuOption {
    // Main menu options
    MainMenuNewGame,
    MainMenuConnect,
    MainMenuQuit,
    // Connect menu options
    ConnectMenuHost,
    ConnectMenuPort,
    ConnectMenuDone,
    ConnectMenuCancel
};

/// Change to a new menu
void changeMenu(Renderer& renderer, Menu*& currentMenu, Menu* newMenu, ClearScreenDrawable* clearDrawable) {
    // Lock renderer (and wait for current frame to end)
    std::lock_guard<std::mutex> rLockGuard(renderer.r_lock);
    
    // Change current menu
    currentMenu = newMenu;
    currentMenu->setCursor(0);
    
    // Rebuild drawables list in renderer
    renderer.clear_drawables();
    renderer.add_drawable(clearDrawable);
    renderer.add_drawable(currentMenu);
}

/// Show exception screen and change back to main menu
void exceptionScreen(Renderer& renderer, Menu*& currentMenu, Menu* mainMenu, ClearScreenDrawable* clearDrawable, std::string message) {
    // Create error menu
    Menu errorMenu(10, 4, renderer.getWidth() / 2, renderer.getHeight() / 2);
    errorMenu.toggleExpand(true);
    errorMenu.toggleCenter(true);
    errorMenu.toggleClamp(true);
    
    static const Formating errorItemFormatting{
        Color::BLACK,
        Color::RED
    };
    std::shared_ptr<MenuItem> headerItem(new MenuItem(0, "Client error! Press space or enter to continue...", errorItemFormatting, errorItemFormatting));
    std::shared_ptr<MenuItem> messageItem(new MenuItem(0, message, errorItemFormatting, errorItemFormatting));
    errorMenu.addItem(headerItem);
    errorMenu.addItem(messageItem);
    
    {
        // Lock renderer
        std::lock_guard<std::mutex> r_lock_guard(renderer.r_lock);
        
        // Set drawables to error menu only
        renderer.clear_drawables();
        renderer.add_drawable(&errorMenu);
    }
    
    // Wait for space or enter
    while(true) {
        char input = renderer.getch();
        if(input == ' ' || input == '\n')
            break;
    }
    
    // Go to main menu
    changeMenu(renderer, currentMenu, mainMenu, clearDrawable);
}

/// Connect client to server, handling exceptions
void connectClientToServer(Renderer& renderer, Menu*& currentMenu, Menu* mainMenu, ClearScreenDrawable* clearDrawable, std::string host, uint16_t port) {
    try {
        // Connect to server
        new GameClient(&renderer, host, port);
    }
    catch(const std::exception& err) {
        exceptionScreen(renderer, currentMenu, mainMenu, clearDrawable, err.what());
        return;
    }
    catch(const std::string err) {
        exceptionScreen(renderer, currentMenu, mainMenu, clearDrawable, err);
        return;
    }
    catch(const char* err) {
        exceptionScreen(renderer, currentMenu, mainMenu, clearDrawable, err);
        return;
    }
    catch(...) {
        exceptionScreen(renderer, currentMenu, mainMenu, clearDrawable, "Unknown exception");
        return;
    }
    
    // Go to main menu
    changeMenu(renderer, currentMenu, mainMenu, clearDrawable);
}

/// Main
int main() {
    // Generate default menu dimensions
    auto defaultMenuW = RENDER_WIDTH / 4;
    auto defaultMenuH = RENDER_HEIGHT / 4;
    auto centerX = RENDER_WIDTH / 2;
    auto centerY = RENDER_HEIGHT / 2;
    
    // Create drawable that clears the screen
    ClearScreenDrawable clearDrawable;
    
    // Create main menu
    Menu mainMenu(defaultMenuW, defaultMenuH, centerX, centerY);
    mainMenu.toggleCenter(true);
    mainMenu.toggleExpand(true);
    mainMenu.toggleClamp(true);
    
    std::shared_ptr<MenuItem> mmNewGame(new MenuItem(MenuOption::MainMenuNewGame, "New game"));
    std::shared_ptr<MenuItem> mmConnect(new MenuItem(MenuOption::MainMenuConnect, "Connect to game"));
    std::shared_ptr<MenuItem> mmQuit(new MenuItem(MenuOption::MainMenuQuit, "Quit"));
    mainMenu.addItem(mmNewGame);
    mainMenu.addItem(mmConnect);
    mainMenu.addItem(mmQuit);
    
    // Create connect to game menu
    Menu connectMenu(defaultMenuW, defaultMenuH, centerX, centerY);
    connectMenu.toggleCenter(true);
    connectMenu.toggleExpand(true);
    connectMenu.toggleClamp(true);
    
    std::shared_ptr<MenuItem> cmHost(new MenuItem(MenuOption::ConnectMenuHost, "Host"));
    std::shared_ptr<MenuItem> cmPort(new MenuItem(MenuOption::ConnectMenuPort, "Port"));
    std::shared_ptr<MenuItem> cmDone(new MenuItem(MenuOption::ConnectMenuDone, "Done"));
    std::shared_ptr<MenuItem> cmCancel(new MenuItem(MenuOption::ConnectMenuCancel, "Cancel"));
    connectMenu.addItem(cmHost);
    connectMenu.addItem(cmPort);
    connectMenu.addItem(cmDone);
    connectMenu.addItem(cmCancel);
    
    // Setup renderer
    Renderer renderer(RENDER_WIDTH, RENDER_HEIGHT);
    Menu* currentMenu;
    changeMenu(renderer, currentMenu, &mainMenu, &clearDrawable);
    auto renderThread = renderer.spawn();
    
    // Input loop
    while(renderer.b_render) {
        // Wait for input
        char input = Renderer::getch();
        
        switch(input) {
            case 'w':
            case 'W':
                // Move cursor up
                currentMenu->moveCursor(-1);
                break;
            case 's':
            case 'S':
                // Move cursor down
                currentMenu->moveCursor(1);
                break;
            case ' ':
            case '\n':
                // Select an option
                auto chosenMenu = currentMenu->selectCursor();
                switch((MenuOption)chosenMenu->getKey()) {
                    case MenuOption::MainMenuNewGame:
                        // TODO
                        break;
                    case MenuOption::MainMenuConnect:
                        changeMenu(renderer, currentMenu, &connectMenu, &clearDrawable);
                        break;
                    case MenuOption::MainMenuQuit:
                        renderer.b_render = false;
                        break;
                    case MenuOption::ConnectMenuHost:
                        // TODO
                        break;
                    case MenuOption::ConnectMenuPort:
                        // TODO
                        break;
                    case MenuOption::ConnectMenuDone:
                        // Connect client to server
                        // TODO get input from other menu items
                        connectClientToServer(renderer, currentMenu, &mainMenu, &clearDrawable, "localhost", 7777);
                        
                        // Client stopped, reset to main menu
                        changeMenu(renderer, currentMenu, &mainMenu, &clearDrawable);
                        break;
                    case MenuOption::ConnectMenuCancel:
                        changeMenu(renderer, currentMenu, &mainMenu, &clearDrawable);
                        break;
                }
                break;
        }
    }
    
    // Wait for render thread to stop
    renderThread.join();
}
