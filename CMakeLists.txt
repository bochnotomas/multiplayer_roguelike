cmake_minimum_required(VERSION 3.0)

project(multiplayer_roguelike)

add_executable(multiplayer_roguelike main.cpp)

install(TARGETS multiplayer_roguelike RUNTIME DESTINATION bin)
