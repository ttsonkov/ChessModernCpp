cmake_minimum_required(VERSION 3.20)
project(ModernChess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

find_package(SFML 2.6 REQUIRED COMPONENTS graphics window system)
find_package(ImGui-SFML REQUIRED)

add_executable(ModernChess
    src/main.cpp
    src/app/Application.cpp
    src/core/Board.cpp
    src/core/ChessGame.cpp
    src/core/Rules.cpp
    src/ui/SfmlRenderer.cpp
)

target_include_directories(ModernChess PRIVATE src)
target_link_libraries(ModernChess
    PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        ImGui-SFML::ImGui-SFML
)

# Ensure 'assets' dir exists in source tree and contains pieces.png
add_custom_command(TARGET ModernChess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:ModernChess>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/assets/pieces.png
        $<TARGET_FILE_DIR:ModernChess>/assets/pieces.png
    COMMENT "Copying assets/pieces.png to output directory"
)
