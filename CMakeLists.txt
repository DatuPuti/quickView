cmake_minimum_required(VERSION 3.16)
project(quickView VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compiler flags for better debugging and warnings
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
# Only find NCurses on non-Windows platforms
if(NOT WIN32)
    find_package(Curses REQUIRED)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
if(NOT WIN32)
    include_directories(${CURSES_INCLUDE_DIRS})
endif()

# Platform-specific source files
if(WIN32)
    set(PLATFORM_SOURCES
        src/platform/terminal_factory.cpp
        src/ui/windows/windows_terminal.cpp
    )
    # Windows-specific libraries
    set(PLATFORM_LIBS)

    # Windows-specific compiler definitions
    add_definitions(-DUNICODE -D_UNICODE)
else()
    set(PLATFORM_SOURCES
        src/platform/terminal_factory.cpp
        src/ui/ncurses/ncurses_terminal.cpp
    )
    set(PLATFORM_LIBS ${CURSES_LIBRARIES})
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/core/quickview.cpp
    src/ui/display.cpp
    src/ui/input.cpp
    src/filesystem/file_operations.cpp
    src/filesystem/image_handler.cpp
    src/utils/utils.cpp
    ${PLATFORM_SOURCES}
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} ${PLATFORM_LIBS})

# Windows-specific target properties
if(WIN32 AND MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()

# Install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Optional: Create uninstall target (commented out - requires cmake_uninstall.cmake.in)
# if(NOT TARGET uninstall)
#     configure_file(
#         "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
#         "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
#         IMMEDIATE @ONLY)
#
#     add_custom_target(uninstall
#         COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
# endif()
