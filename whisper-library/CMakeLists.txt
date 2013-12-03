FIND_PACKAGE (Boost 1.54 COMPONENT REQUIRED)

set(SOURCES
	src/socketconnector.cpp
)

set(HEADERS
	include/whisper-library/socketconnector.hpp
	include/whisper-library/common.hpp
)

include_directories(
    include/whisper-library
	${Boost_INCLUDE_DIR}
)

TARGET_LINK_LIBRARIES(
	whisper_library ${Boost_LIBRARIES}
)

add_library(whisper-library SHARED
    ${HEADERS}
    ${SOURCES}
)

add_definitions(-DWHISPER_BUILD)

install(TARGETS whisper-library RUNTIME DESTINATION .
				LIBRARY DESTINATION .)
