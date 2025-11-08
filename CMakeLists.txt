cmake_minimum_required(VERSION 3.15)

project(RayTracerRecursivo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  "src/*.cpp"
)

add_executable(raytracer
  ${SOURCES}
  src/main.cpp
)

target_include_directories(raytracer PRIVATE src)


