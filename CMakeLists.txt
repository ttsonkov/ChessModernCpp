cmake_minimum_required(VERSION 3.20)
project(ModernChess 
    VERSION 1.0.0
    DESCRIPTION "Modern chess application with SFML"
    LANGUAGES CXX
)

# ============================================================================
# Build Settings
# ============================================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================
# Dependencies
# ============================================================================

include(FetchContent)

# Try to find SFML locally first, otherwise fetch it
find_package(SFML 2.6 QUIET COMPONENTS graphics window system)

if(NOT SFML_FOUND)
    message(STATUS "SFML not found locally, fetching from GitHub...")
    
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
        GIT_SHALLOW TRUE
    )
    
    # SFML options
    set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(SFML)
endif()

# Try to find ImGui-SFML locally first, otherwise fetch it
find_package(ImGui-SFML QUIET)

if(NOT ImGui-SFML_FOUND)
    message(STATUS "ImGui-SFML not found locally, fetching from GitHub...")
    
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.90.1
        GIT_SHALLOW TRUE
    )
    
    FetchContent_Declare(
        imgui-sfml
        GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
        GIT_TAG 2.6.x
        GIT_SHALLOW TRUE
    )
    
    # ImGui-SFML options
    set(IMGUI_DIR ${CMAKE_BINARY_DIR}/_deps/imgui-src CACHE PATH "" FORCE)
    set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "" FORCE)
    set(IMGUI_SFML_IMGUI_DEMO ON CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(imgui imgui-sfml)
endif()

# ============================================================================
# Source Files
# ============================================================================

set(CORE_SOURCES
    src/core/Board.cpp
    src/core/ChessGame.cpp
    src/core/Rules.cpp
)

set(CORE_HEADERS
    src/core/Types.hpp
    src/core/Fwd.hpp
    src/core/Piece.hpp
    src/core/Move.hpp
    src/core/Board.hpp
    src/core/IGame.hpp
    src/core/ChessGame.hpp
    src/core/Rules.hpp
)

set(UI_SOURCES
    src/ui/SfmlRenderer.cpp
    src/ui/SfmlInputHandler.cpp
)

set(UI_HEADERS
    src/ui/IRenderer.hpp
    src/ui/IInputHandler.hpp
    src/ui/SfmlRenderer.hpp
    src/ui/SfmlInputHandler.hpp
)

set(APP_SOURCES
    src/app/Application.cpp
)

set(APP_HEADERS
    src/app/Application.hpp
)

set(CONFIG_HEADERS
    src/Config.hpp
)

# ============================================================================
# Executable
# ============================================================================

add_executable(${PROJECT_NAME}
    src/main.cpp
    ${CORE_SOURCES}
    ${CORE_HEADERS}
    ${UI_SOURCES}
    ${UI_HEADERS}
    ${APP_SOURCES}
    ${APP_HEADERS}
    ${CONFIG_HEADERS}
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        ImGui-SFML::ImGui-SFML
)

# Compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ============================================================================
# Assets
# ============================================================================

# Copy assets to output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory 
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/assets/pieces.png"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/pieces.png"
    COMMENT "Copying assets to output directory"
)

# ============================================================================
# IDE Support
# ============================================================================

# Group source files in IDEs
source_group("Core" FILES ${CORE_SOURCES} ${CORE_HEADERS})
source_group("UI" FILES ${UI_SOURCES} ${UI_HEADERS})
source_group("App" FILES ${APP_SOURCES} ${APP_HEADERS})
source_group("Config" FILES ${CONFIG_HEADERS})
