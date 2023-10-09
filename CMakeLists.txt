cmake_minimum_required(VERSION 3.12)

project(IsingFantasyMapGenerator)

# Find SDL2 package
find_package(SDL2 REQUIRED)

# Include header files from the 'include' directory
include_directories(include)

# Create a list of your source files in the 'src' directory
set(SOURCE_FILES
    main.cpp
    src/spin_lattice.cpp
    src/map.cpp
    src/renderer.cpp
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link SDL2 library
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
