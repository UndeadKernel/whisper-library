cmake_minimum_required(VERSION 2.8)

project(example)

set(SOURCES
    main.cpp
)

# Set include directories.
include_directories(
    ../whisperLibrary/include
    
)

add_executable(example
    ${SOURCES}
)

add_dependencies(example
    whisperLibrary
)

target_link_libraries(example
    whisperLibrary
)

install(TARGETS example RUNTIME DESTINATION .)