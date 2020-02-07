cmake_minimum_required(VERSION 3.0)

project(multiplayer_roguelike)

set(CMAKE_CXX_STANDARD 14)

add_executable(multiplayer_roguelike main.cpp src/networking/buffer.cpp src/networking/game_event.cpp src/networking/socket_exception.cpp src/server/server.cpp src/client/client.cpp)

