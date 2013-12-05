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