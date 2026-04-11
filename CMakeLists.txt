
cmake_minimum_required(VERSION 3.15)
project(TypingShooter)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(ext/raylib)
include_directories(ext/nlohmann)

file(GLOB SRC_FILES src/*.cpp)

add_executable(game ${SRC_FILES})

target_link_libraries(game
    ${CMAKE_SOURCE_DIR}/ext/raylib/libraylib.a
    opengl32
    gdi32
    winmm
)