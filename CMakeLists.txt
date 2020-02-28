cmake_minimum_required(VERSION 3.1)

project(multiplayer_roguelike)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads)

add_executable(networking_example example/networking.cpp src/client/Client.cpp src/networking/Buffer.cpp src/networking/ClientMessage.cpp src/networking/ServerMessage.cpp src/networking/Socket.cpp src/networking/SocketSelector.cpp src/networking/SocketException.cpp src/server/Player.cpp src/server/Server.cpp src/client/Object.cpp src/client/Client.hpp src/networking/Buffer.hpp src/networking/ClientMessage.hpp src/networking/ServerMessage.hpp src/networking/Socket.hpp src/networking/SocketSelector.hpp src/networking/SocketException.hpp src/server/Player.hpp src/server/Server.hpp src/client/Object.h)

add_executable(engine example/client.cpp src/client/Map.cpp src/client/Object.cpp src/client/Renderer.cpp src/client/Camera.cpp src/client/Menu.cpp src/client/MenuItem.cpp src/client/Map.h src/client/Object.h src/client/Renderer.h src/client/Camera.h src/client/Menu.hpp src/client/MenuItem.hpp)

add_executable(server example/levelGeneration.cpp src/server/LevelGeneration2D.cpp src/server/LevelGeneration2D.h)

# networking_example
if (WIN32)
    # Link with winsock2 if on Windows
    target_link_libraries(networking_example ws2_32 wsock32 ${CMAKE_THREAD_LIBS_INIT})
else()
    target_link_libraries(networking_example ${CMAKE_THREAD_LIBS_INIT})
endif()

# engine
target_link_libraries(engine ${CMAKE_THREAD_LIBS_INIT})
