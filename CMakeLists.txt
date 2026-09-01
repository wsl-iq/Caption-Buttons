cmake_minimum_required(VERSION 3.15)
project(macos_window VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library
add_library(macos_window STATIC
    src/macos_window.cpp
)

target_include_directories(macos_window PUBLIC include)
target_link_libraries(macos_window PUBLIC
    gdi32
    user32
    gdiplus
    dwmapi
)

# Examples
add_executable(example_cpp examples/example_cpp.cpp)
target_link_libraries(example_cpp PRIVATE macos_window)

add_executable(example_c examples/example_c.c)
target_link_libraries(example_c PRIVATE macos_window)

# Windows-specific settings
if(WIN32)
    set_target_properties(example_cpp example_c PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()