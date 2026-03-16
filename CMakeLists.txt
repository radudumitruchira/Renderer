cmake_minimum_required(VERSION 3.20)

project(Renderer)

add_subdirectory(vendor/glfw)
add_executable(Renderer src/main.cpp)

target_compile_options(Renderer PRIVATE /W4 /IGNORE:C5287)

target_link_libraries(Renderer d3d11 glfw)
target_include_directories(Renderer PRIVATE vendor/glfw/include)
