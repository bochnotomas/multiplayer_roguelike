cmake_minimum_required(VERSION 3.0)

project(multiplayer_roguelike)

find_package(Threads)

set(CMAKE_CXX_STANDARD 14)

add_executable(networking_example example/networking.cpp src/client/client.cpp src/networking/buffer.cpp src/networking/client_message.cpp src/networking/server_message.cpp src/networking/socket_exception.cpp src/server/player.cpp src/server/server.cpp)

target_link_libraries(networking_example ${CMAKE_THREAD_LIBS_INIT})
