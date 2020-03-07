#include "GameClient.hpp"
#include "Menu.hpp"

void GameClient::logic() {
    throw std::runtime_error("Test exception because client does nothing for now...");
}

GameClient::GameClient(Renderer* renderer, std::string host, uint16_t port) :
    Client(host, port, 5000), // 5s timeout
    renderer(renderer)
{
    {
        // Lock renderer
        std::lock_guard<std::mutex> r_lock_guard(renderer->r_lock);
        
        // Clear drawables
        renderer->clear_drawables();
    }
    
    // Start the client logic
    logic();
    
    // Lock renderer
    std::lock_guard<std::mutex> r_lock_guard(renderer->r_lock);
    
    // Clear drawables (so the renderer doesn't try to draw anything invalid)
    renderer->clear_drawables();
}
