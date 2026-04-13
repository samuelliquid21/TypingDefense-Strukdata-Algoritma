cmake_minimum_required(VERSION 3.15)
project(TypingShooter)

set(CMAKE_CXX_STANDARD 17)

# 1. Penanganan Assets & Data
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/data" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

# 2. Include Directories
include_directories(include)
include_directories(ext/nlohmann)

file(GLOB SRC_FILES src/*.cpp)
add_executable(game ${SRC_FILES})

# 3. Logika Cross-Platform untuk Raylib
if(WIN32)
    # Konfigurasi Windows (Tetap menggunakan file lokal di folder ext)
    include_directories(ext/raylib)
    target_link_libraries(game
        ${CMAKE_SOURCE_DIR}/ext/raylib/libraylib.a
        opengl32
        gdi32
        winmm
    )
elseif(UNIX AND NOT APPLE)
    # Konfigurasi Linux (Menggunakan library dari sistem)
    # Mencari raylib di sistem menggunakan find_package
    find_package(raylib REQUIRED)
    
    target_link_libraries(game
        raylib
        m       
        dl      
        pthread 
    )
endif()
