set(SOURCES
	src/socketconnector.cpp
	src/channelmanager.cpp
)

set(HEADERS
	include/whisperLibrary/socketconnector.hpp
	include/whisperLibrary/common.hpp
	include/whisperLibrary/channelmanager.hpp
	include/whisperLibrary/covertchannel.hpp
	include/whisperLibrary/encoder.hpp
	include/whisperLibrary/decoder.hpp
)

# Boost library.
set(Boost_USE_STATIC_LIBS ON)               # Use static libs.
add_definitions(-DBOOST_ALL_NO_LIB)         # Disable auto-linking in MSVC.
find_package(Boost 1.54.0 QUIET COMPONENTS filesystem system)

include_directories(
    include/whisperLibrary
	${Boost_INCLUDE_DIRS}
)

add_library(whisperLibrary SHARED
    ${HEADERS}
    ${SOURCES}
)

TARGET_LINK_LIBRARIES(whisperLibrary
	${Boost_LIBRARIES}
)

add_definitions(-DWHISPER_BUILD)

install(TARGETS whisperLibrary RUNTIME DESTINATION .
				LIBRARY DESTINATION .)