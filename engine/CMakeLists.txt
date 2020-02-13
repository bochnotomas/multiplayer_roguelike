cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14) 

# set the project name
project(engine)

find_package(Threads)

# add the executable
add_executable(engine src/Main.cpp src/Map.cpp src/Object.cpp src/Renderer.cpp src/Camera.cpp)

target_link_libraries(engine ${CMAKE_THREAD_LIBS_INIT})
