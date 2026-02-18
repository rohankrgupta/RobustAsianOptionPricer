cmake_minimum_required(VERSION 3.20)
project(cpp20_practice LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(app src/main.cpp)

target_compile_options(app PRIVATE
  -Wall -Wextra -Wpedantic -Wshadow -Wconversion
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(app PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -g)
  target_link_options(app PRIVATE -fsanitize=address,undefined)
endif()
