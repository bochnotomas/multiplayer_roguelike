#include <vector>

struct TexturePoint{
    char character;
	Formating formating;
};

class Texture{
public:
    Texture(){
    m_plane = {
        {{'A', {Color::WHITE, Color::BLACK}}, {'A', {Color::WHITE, Color::BLACK}}, {'A', {Color::WHITE, Color::BLACK}}, {'A', {Color::WHITE, Color::BLACK}}, {'A', {Color::WHITE, Color::BLACK}}},
        {{'B', {Color::WHITE, Color::BLACK}}, {'B', {Color::WHITE, Color::BLACK}}, {'B', {Color::WHITE, Color::BLACK}}, {'B', {Color::WHITE, Color::BLACK}}, {'B', {Color::WHITE, Color::BLACK}}},
        {{'C', {Color::WHITE, Color::BLACK}}, {'C', {Color::WHITE, Color::BLACK}}, {'C', {Color::WHITE, Color::BLACK}}, {'C', {Color::WHITE, Color::BLACK}}, {'C', {Color::WHITE, Color::BLACK}}},
        {{'D', {Color::WHITE, Color::BLACK}}, {'D', {Color::WHITE, Color::BLACK}}, {'D', {Color::WHITE, Color::BLACK}}, {'D', {Color::WHITE, Color::BLACK}}, {'D', {Color::WHITE, Color::BLACK}}},
        {{'E', {Color::WHITE, Color::BLACK}}, {'E', {Color::WHITE, Color::BLACK}}, {'E', {Color::WHITE, Color::BLACK}}, {'E', {Color::WHITE, Color::BLACK}}, {'E', {Color::WHITE, Color::BLACK}}},
    };
    }
    Texture(std::vector<std::vector<TexturePoint>> &texture) : m_plane(texture){};
    std::vector<std::vector<TexturePoint>> get_plane() const {
        return m_plane;
    }
private:
    std::vector<std::vector<TexturePoint>> m_plane;
};