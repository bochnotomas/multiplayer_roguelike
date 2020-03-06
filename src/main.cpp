#include "client/Menu.hpp"
#include "client/Client.hpp"
#include "server/LevelGeneration2D.h"

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

/// Main
int main() {
    // Generate default menu dimensions
    auto defaultMenuW = RENDER_WIDTH / 4;
    auto defaultMenuH = RENDER_HEIGHT / 4;
    auto centerX = RENDER_WIDTH / 2;
    auto centerY = RENDER_HEIGHT / 2;
    
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
    Menu* currentMenu = &mainMenu;
    renderer.add_drawable(currentMenu);
    auto renderThread = renderer.spawn();
    
    // Input loop
    while(renderer.b_render) {
        // Wait for input
        switch(Renderer::getch()) {
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
                        currentMenu = &connectMenu;
                        currentMenu->setCursor(0);
                        renderer.clear_drawables();
                        renderer.add_drawable(currentMenu);
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
                        // TODO
                        break;
                    case MenuOption::ConnectMenuCancel:
                        currentMenu = &mainMenu;
                        currentMenu->setCursor(0);
                        renderer.clear_drawables();
                        renderer.add_drawable(currentMenu);
                        break;
                }
                break;
        }
    }
    
    // Wait for render thread to stop
    renderThread.join();
}
