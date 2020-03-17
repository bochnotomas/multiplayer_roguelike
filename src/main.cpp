#include "client/Menu.hpp"
#include "server/LevelGeneration2D.h"
#include "client/GameClient.hpp"
#include "server/GameServer.hpp"
#include "client/ClearScreenDrawable.hpp"
#include "client/InputMenuItem.hpp"

// num of columns rendered on the screen
constexpr unsigned short RENDER_WIDTH = 100;
// num of rows rendered on the screen
constexpr unsigned short RENDER_HEIGHT = 30;

constexpr Formating errorItemFormatting{
    Color::WHITE,
    Color::RED
};

enum MenuOption {
    Blank,
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

/// Show popup screen and clear drawables
void popup(Renderer& renderer, const Formating& formatting, std::string header, std::string message) {
    // Create popup menu
    std::shared_ptr<Menu> errorMenu(new Menu(10, 4, renderer.getWidth() / 2, renderer.getHeight() / 2));
    errorMenu->toggleCenter(true);
    
    std::shared_ptr<MenuItem> headerItem(new MenuItem(0, header, false, formatting));
    std::shared_ptr<MenuItem> messageItem(new MenuItem(0, message, false, formatting));
    errorMenu->addItem(headerItem);
    errorMenu->addItem(messageItem);
    
    {
        // Lock renderer
        std::lock_guard<std::mutex> r_lock_guard(renderer.r_lock);
        
        // Set drawables to popup menu only
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

/// Show exception screen
void exceptionScreen(Renderer& renderer, std::string message) {
    popup(renderer, errorItemFormatting, "Network error! Press space or enter to continue...", message);
}

/// Show invalid input screen
void invalidInputScreen(Renderer& renderer, std::string message) {
    popup(renderer, errorItemFormatting, "Invalid input! Press space or enter to continue...", message);
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

/// Parse port number and show invalid input popup if not valid
bool validatePort(Renderer& renderer, std::string input, int& output) {
    auto portText = input;
    size_t idx;
    int chosenPort = std::stoi(portText, &idx);
    if(idx != portText.size() || idx < 0 || idx > 65535) {
        // Popup
        invalidInputScreen(renderer, "Port number must be a valid number between 0 and 65535");
        return false;
    }
    
    return true;
}

/// Main
int main() {
    // Init networking
    Socket::initSocketApi();
    
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
    
    std::shared_ptr<MenuItem> blankItem(new MenuItem(MenuOption::Blank, "", false));
    std::shared_ptr<MenuItem> mmNewGame(new MenuItem(MenuOption::MainMenuNewGame, "New game"));
    std::shared_ptr<MenuItem> mmConnect(new MenuItem(MenuOption::MainMenuConnect, "Connect to game"));
    std::shared_ptr<MenuItem> mmQuit(new MenuItem(MenuOption::MainMenuQuit, "Quit"));
    mainMenu->addItem(mmNewGame);
    mainMenu->addItem(mmConnect);
    mainMenu->addItem(blankItem);
    mainMenu->addItem(mmQuit);
    
    // Create new game menu
    std::shared_ptr<Menu> newMenu(new Menu(defaultMenuW, defaultMenuH, centerX, centerY));
    newMenu->toggleCenter(true);
    
    std::shared_ptr<InputMenuItem> nmPort(new InputMenuItem(MenuOption::NewMenuPort, "Port", "9999")); 
    std::shared_ptr<MenuItem> nmDone(new MenuItem(MenuOption::NewMenuDone, "Start"));
    std::shared_ptr<MenuItem> nmCancel(new MenuItem(MenuOption::NewMenuCancel, "Cancel"));
    newMenu->addItem(nmPort);
    newMenu->addItem(blankItem);
    newMenu->addItem(nmDone);
    newMenu->addItem(nmCancel);
    
    // Create connect to game menu
    std::shared_ptr<Menu> connectMenu(new Menu(defaultMenuW, defaultMenuH, centerX, centerY));
    connectMenu->toggleCenter(true);
    
    std::shared_ptr<InputMenuItem> cmHost(new InputMenuItem(MenuOption::ConnectMenuHost, "Host", "localhost"));
    std::shared_ptr<InputMenuItem> cmPort(new InputMenuItem(MenuOption::ConnectMenuPort, "Port", "9999"));
    std::shared_ptr<MenuItem> cmDone(new MenuItem(MenuOption::ConnectMenuDone, "Connect"));
    std::shared_ptr<MenuItem> cmCancel(new MenuItem(MenuOption::ConnectMenuCancel, "Cancel"));
    connectMenu->addItem(cmHost);
    connectMenu->addItem(cmPort);
    connectMenu->addItem(blankItem);
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
        
        if(!currentMenu->input(input)) {
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
                            nmPort->set("9999");
                            changeMenu(renderer, currentMenu, newMenu, clearDrawable);
                            break;
                        case MenuOption::MainMenuConnect:
                            cmHost->set("localhost");
                            cmPort->set("9999");
                            changeMenu(renderer, currentMenu, connectMenu, clearDrawable);
                            break;
                        case MenuOption::MainMenuQuit:
                            renderer.b_render = false;
                            break;
                        case MenuOption::NewMenuDone:
                            {
                                // Validate port number
                                int port;
                                if(!validatePort(renderer, nmPort->get(), port)) {
                                    // Go back to same menu, since popup clears
                                    // the menu list
                                    changeMenu(renderer, currentMenu, newMenu, clearDrawable);
                                    break;
                                }
                                
                                // Stop server if already running
                                if(server)
                                    server->stop();
                                
                                // Start server
                                server = createServer(renderer, port);
                                
                                // Connect client to server if server started
                                if(server)
                                    connectClientToServer(renderer, "localhost", port);
                                
                                // Client stopped, reset to main menu
                                changeMenu(renderer, currentMenu, mainMenu, clearDrawable);
                            }
                            break;
                        case MenuOption::NewMenuCancel:
                            changeMenu(renderer, currentMenu, mainMenu, clearDrawable);
                            break;
                        case MenuOption::ConnectMenuDone:
                            // Validate port number
                            int port;
                            if(!validatePort(renderer, cmPort->get(), port)) {
                                // Go back to same menu, since popup clears
                                // the menu list
                                changeMenu(renderer, currentMenu, connectMenu, clearDrawable);
                                break;
                            }
                            
                            // Connect client to server
                            connectClientToServer(renderer, cmHost->get(), port);
                            
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
    }

    // Wait for render thread to stop
    renderThread.join();
    
    // Stop server if still running
    if(server)
        server->stop();
    
    // Cleanup networking
    Socket::cleanupSocketApi();
}
