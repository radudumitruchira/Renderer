cmake_minimum_required(VERSION 3.20)

project(Renderer)

add_executable(Renderer src/main.cpp)

target_compile_options(Renderer PRIVATE /W4)

target_link_libraries(Renderer d3d11)
