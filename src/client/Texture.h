#ifndef ROGUELIKE_TEXTURE_H_INCLUDED
#define ROGUELIKE_TEXTURE_H_INCLUDED
#include <vector>

struct TexturePoint{
    char character;
	Formating formating;
};

class Texture{
public:
    Texture(){
    m_plane = {
        {{'A', {Color::WHITE, Color::WHITE}}, {'A', {Color::WHITE, Color::WHITE}}, {'A', {Color::WHITE, Color::WHITE}}, {'A', {Color::WHITE, Color::WHITE}}, {'A', {Color::WHITE, Color::WHITE}}, {'A', {Color::WHITE, Color::WHITE}}},
        {{'B', {Color::WHITE, Color::WHITE}}, {'B', {Color::WHITE, Color::WHITE}}, {'B', {Color::WHITE, Color::WHITE}}, {'B', {Color::WHITE, Color::WHITE}}, {'B', {Color::WHITE, Color::WHITE}}, {'B', {Color::WHITE, Color::WHITE}}},
        {{'C', {Color::WHITE, Color::WHITE}}, {'C', {Color::WHITE, Color::WHITE}}, {'C', {Color::WHITE, Color::WHITE}}, {'C', {Color::WHITE, Color::WHITE}}, {'C', {Color::WHITE, Color::WHITE}}, {'C', {Color::WHITE, Color::WHITE}}},
        {{'P', {Color::BLACK, Color::RED}}, {'O', {Color::BLACK, Color::RED}}, {'L', {Color::BLACK, Color::RED}}, {'A', {Color::BLACK, Color::RED}}, {'N', {Color::BLACK, Color::RED}}, {'D', {Color::BLACK, Color::RED}}},
        {{'E', {Color::RED, Color::RED}}, {'E', {Color::RED, Color::RED}}, {'E', {Color::RED, Color::RED}}, {'E', {Color::RED, Color::RED}}, {'E', {Color::RED, Color::RED}}, {'E', {Color::RED, Color::RED}}},
        {{'F', {Color::RED, Color::RED}}, {'F', {Color::RED, Color::RED}}, {'F', {Color::RED, Color::RED}}, {'F', {Color::RED, Color::RED}}, {'F', {Color::RED, Color::RED}}, {'F', {Color::RED, Color::RED}}},
    };
    }
    Texture(std::vector<std::vector<TexturePoint>> &texture) : m_plane(texture){};
    Texture(std::vector<std::vector<TexturePoint>> &&texture) : m_plane(texture){};
    std::vector<std::vector<TexturePoint>> get_plane() const {
        return m_plane;
    }
private:
    std::vector<std::vector<TexturePoint>> m_plane;
};
#endif
