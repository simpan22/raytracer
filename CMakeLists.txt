cmake_minimum_required(VERSION 3.12)
project(raytracer LANGUAGES CXX)

#find_package(SDL2 REQUIRED)

add_executable(raytracer "ppma_io.cpp" "main.cpp")

set_property(TARGET raytracer PROPERTY CXX_STANDARD 17)

#target_link_libraries(raytracer PRIVATE SDL2)
