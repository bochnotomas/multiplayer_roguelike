cmake_minimum_required(VERSION 3.1)

project(multiplayer_roguelike)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads)

add_executable(networking_example example/networking.cpp src/client/client.cpp src/networking/buffer.cpp src/networking/client_message.cpp src/networking/server_message.cpp src/networking/socket.cpp src/networking/socket_select.cpp src/networking/socket_exception.cpp src/server/player.cpp src/server/server.cpp src/client/Object.cpp src/client/client.hpp src/networking/buffer.hpp src/networking/client_message.hpp src/networking/server_message.hpp src/networking/socket.hpp src/networking/socket_select.hpp src/networking/socket_exception.hpp src/server/player.hpp src/server/server.hpp src/client/Object.h)

add_executable(engine src/client/Main.cpp src/client/Map.cpp src/client/Object.cpp src/client/Renderer.cpp src/client/Camera.cpp src/client/Map.h src/client/Object.h src/client/Renderer.h src/client/Camera.h)

# networking_example
if (WIN32)
    # Link with winsock2 if on Windows
    target_link_libraries(networking_example ws2_32 wsock32 ${CMAKE_THREAD_LIBS_INIT})
else()
    target_link_libraries(networking_example ${CMAKE_THREAD_LIBS_INIT})
endif()

# engine
target_link_libraries(engine ${CMAKE_THREAD_LIBS_INIT})
