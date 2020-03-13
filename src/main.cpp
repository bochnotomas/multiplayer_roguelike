#include "client/Menu.hpp"
#include "server/LevelGeneration2D.h"
#include "client/GameClient.hpp"
#include "server/GameServer.hpp"
#include "client/ClearScreenDrawable.hpp"

// num of columns rendered on the screen
constexpr unsigned short RENDER_WIDTH = 100;
// num of rows rendered on the screen
constexpr unsigned short RENDER_HEIGHT = 30;

enum MenuOption {
    // Main menu options
    MainMenuNewGame,
    MainMenuConnect,
    MainMenuQuit,
    // New game options
    NewMenuPort,
    NewMenuDone,
    NewMenuCancel,
    // Connect menu options
    ConnectMenuHost,
    ConnectMenuPort,
    ConnectMenuDone,
    ConnectMenuCancel
};

/// Change to a new menu
void changeMenu(Renderer& renderer, std::shared_ptr<Menu>& currentMenu, std::shared_ptr<Menu> newMenu, std::shared_ptr<ClearScreenDrawable> clearDrawable) {
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
void exceptionScreen(Renderer& renderer, std::string message) {
    // Create error menu
    std::shared_ptr<Menu> errorMenu(new Menu(10, 4, renderer.getWidth() / 2, renderer.getHeight() / 2));
    errorMenu->toggleCenter(true);
    
    static const Formating errorItemFormatting{
        Color::WHITE,
        Color::RED
    };
    std::shared_ptr<MenuItem> headerItem(new MenuItem(0, "Network error! Press space or enter to continue...", errorItemFormatting, errorItemFormatting));
    std::shared_ptr<MenuItem> messageItem(new MenuItem(0, message, errorItemFormatting, errorItemFormatting));
    errorMenu->addItem(headerItem);
    errorMenu->addItem(messageItem);
    
    {
        // Lock renderer
        std::lock_guard<std::mutex> r_lock_guard(renderer.r_lock);
        
        // Set drawables to error menu only
        renderer.clear_drawables();
        renderer.add_drawable(errorMenu);
    }
    
    // Wait for space or enter
    while(true) {
        char input = renderer.getch();
        if(input == ' ' || input == '\n')
            break;
    }
}

/// Connect client to server, handling network exceptions
void connectClientToServer(Renderer& renderer, std::string host, uint16_t port) {
    try {
        // Connect to server
        GameClient(&renderer, host, port);
    }
    catch(const SocketException& err) {
        exceptionScreen(renderer, err.what());
        return;
    }
}

/// Create a new server, handling network exceptions
std::unique_ptr<GameServer> createServer(Renderer& renderer, uint16_t port) {
    std::unique_ptr<GameServer> server = nullptr;
    try {
        // Create server and start thread
        server = std::unique_ptr<GameServer>(new GameServer(port));
        server->start();
    }
    catch(const SocketException& err) {
        exceptionScreen(renderer, err.what());
        return nullptr;
    }
    
    return server;
}

/// Main
int main() {
    // Generate default menu dimensions
    auto defaultMenuW = RENDER_WIDTH / 4;
    auto defaultMenuH = RENDER_HEIGHT / 4;
    auto centerX = RENDER_WIDTH / 2;
    auto centerY = RENDER_HEIGHT / 2;
    
    // Create drawable that clears the screen
    std::shared_ptr<ClearScreenDrawable> clearDrawable(new ClearScreenDrawable());
    
    // Create main menu
    std::shared_ptr<Menu> mainMenu(new Menu(defaultMenuW, defaultMenuH, centerX, centerY));
    mainMenu->toggleCenter(true);
    
    std::shared_ptr<MenuItem> mmNewGame(new MenuItem(MenuOption::MainMenuNewGame, "New game"));
    std::shared_ptr<MenuItem> mmConnect(new MenuItem(MenuOption::MainMenuConnect, "Connect to game"));
    std::shared_ptr<MenuItem> mmQuit(new MenuItem(MenuOption::MainMenuQuit, "Quit"));
    mainMenu->addItem(mmNewGame);
    mainMenu->addItem(mmConnect);
    mainMenu->addItem(mmQuit);
    
    // Create new game menu
    std::shared_ptr<Menu> newMenu(new Menu(defaultMenuW, defaultMenuH, centerX, centerY));
    newMenu->toggleCenter(true);
    
    std::shared_ptr<MenuItem> nmPort(new MenuItem(MenuOption::NewMenuPort, "Port")); // TODO text input
    std::shared_ptr<MenuItem> nmDone(new MenuItem(MenuOption::NewMenuDone, "Done"));
    std::shared_ptr<MenuItem> nmCancel(new MenuItem(MenuOption::NewMenuCancel, "Cancel"));
    newMenu->addItem(nmPort);
    newMenu->addItem(nmDone);
    newMenu->addItem(nmCancel);
    
    // Create connect to game menu
    std::shared_ptr<Menu> connectMenu(new Menu(defaultMenuW, defaultMenuH, centerX, centerY));
    connectMenu->toggleCenter(true);
    
    std::shared_ptr<MenuItem> cmHost(new MenuItem(MenuOption::ConnectMenuHost, "Host")); // TODO text input
    std::shared_ptr<MenuItem> cmPort(new MenuItem(MenuOption::ConnectMenuPort, "Port")); // TODO text input
    std::shared_ptr<MenuItem> cmDone(new MenuItem(MenuOption::ConnectMenuDone, "Done"));
    std::shared_ptr<MenuItem> cmCancel(new MenuItem(MenuOption::ConnectMenuCancel, "Cancel"));
    connectMenu->addItem(cmHost);
    connectMenu->addItem(cmPort);
    connectMenu->addItem(cmDone);
    connectMenu->addItem(cmCancel);
    
    // Setup renderer
    Renderer renderer(RENDER_WIDTH, RENDER_HEIGHT);
    std::shared_ptr<Menu> currentMenu;
    changeMenu(renderer, currentMenu, mainMenu, clearDrawable);
    auto renderThread = renderer.spawn();
    
    // Input loop
    std::unique_ptr<GameServer> server = nullptr;
    while(renderer.b_render) {
        // Wait for input
        char input = Renderer::getch();
        
        switch(input) {
            case 'w':
            case 'W':
            case 'r':
            case 'R':
                // Move cursor up
                currentMenu->moveCursor(-1);
                break;
            case 's':
            case 'S':
            case 'f':
            case 'F':
                // Move cursor down
                currentMenu->moveCursor(1);
                break;
            case ' ':
            case '\n':
            case '\r':
                // Select an option
                auto chosenMenu = currentMenu->selectCursor();
                switch((MenuOption)chosenMenu->getKey()) {
                    case MenuOption::MainMenuNewGame:
                        changeMenu(renderer, currentMenu, newMenu, clearDrawable);
                        break;
                    case MenuOption::MainMenuConnect:
                        changeMenu(renderer, currentMenu, connectMenu, clearDrawable);
                        break;
                    case MenuOption::MainMenuQuit:
                        renderer.b_render = false;
                        break;
                    case MenuOption::NewMenuPort:
                        // TODO
                        break;
                    case MenuOption::NewMenuDone:
                        // Stop server if already running
                        if(server)
                            server->stop();
                        
                        // Start server
                        // TODO get input from port menu item
                        server = createServer(renderer, 7777);
                        
                        // Connect client to server if server started
                        if(server)
                            connectClientToServer(renderer, "localhost", 7777);
                        
                        // Client stopped, reset to main menu
                        changeMenu(renderer, currentMenu, mainMenu, clearDrawable);
                        break;
                    case MenuOption::NewMenuCancel:
                        changeMenu(renderer, currentMenu, mainMenu, clearDrawable);
                        break;
                    case MenuOption::ConnectMenuHost:
                        // TODO
                        break;
                    case MenuOption::ConnectMenuPort:
                        // TODO
                        break;
                    case MenuOption::ConnectMenuDone:
                        // Connect client to server
                        // TODO get input from host and port menu items
                        connectClientToServer(renderer, "localhost", 7777);
                        
                        // Client stopped, reset to main menu
                        changeMenu(renderer, currentMenu, mainMenu, clearDrawable);
                        break;
                    case MenuOption::ConnectMenuCancel:
                        changeMenu(renderer, currentMenu, mainMenu, clearDrawable);
                        break;
                }
                break;
        }
    }

    // Wait for render thread to stop
    renderThread.join();
    
    // Stop server if still running
    if(server) {
        std::cout << "Waiting for server to die..." << std::endl;
        server->stop();
        std::cout << "OK!" << std::endl;
    }
}
