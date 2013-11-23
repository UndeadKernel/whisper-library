set(SOURCES
	src/socketconnector.cpp
)

set(HEADERS
	include/whisper-library/socketconnector.hpp
	include/whisper-library/common.hpp
)

include_directories(
    include/whisper-library
)

add_library(whisper-library SHARED
    ${HEADERS}
    ${SOURCES}
)

add_definitions(-DWHISPER_BUILD)

install(TARGETS whisper-library RUNTIME DESTINATION .)