set(SOURCES
    main.cpp
)

# Set include directories.
include_directories(
    ../whisper-library/include
    
)

add_executable(example
    ${SOURCES}
)

add_dependencies(example
    whisper-library
)

target_link_libraries(example
    whisper-library
)