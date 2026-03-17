cmake_minimum_required(VERSION 3.20)

project(Renderer)

add_subdirectory(vendor/glfw)
add_executable(Renderer src/main.cpp shader/vs.hlsl shader/ps.hlsl)

set_source_files_properties(shader/vs.hlsl PROPERTIES VS_SHADER_TYPE Vertex)
set_source_files_properties(shader/ps.hlsl PROPERTIES VS_SHADER_TYPE Pixel)

target_compile_options(Renderer PRIVATE /W4 /IGNORE:C5287)

target_link_libraries(Renderer d3d11 glfw D3DCompiler)
target_include_directories(Renderer PRIVATE vendor/glfw/include)
