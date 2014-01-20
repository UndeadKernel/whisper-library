cmake_minimum_required(VERSION 2.8)

project(whisperLibrary)

set(SOURCES
	src/socketconnector.cpp
	src/channelmanager.cpp
	src/bitSetEncoder.cpp
	src/bitSetDecoder.cpp
	src/tcppacket.cpp
)

set(HEADERS
	include/whisperLibrary/socketconnector.hpp
	include/whisperLibrary/common.hpp
	include/whisperLibrary/channelmanager.hpp
	include/whisperLibrary/covertchannel.hpp
	include/whisperLibrary/encoder.hpp
	include/whisperLibrary/bitSetEncoder.hpp
	include/whisperLibrary/decoder.hpp
	include/whisperLibrary/bitSetDecoder.hpp
	include/whisperLibrary/tcppacket.hpp
)

set(TESTS
	tests/tcpPacketTest.cpp
	tests/bitSetEncoderTest.cpp
	tests/bitSetDecoderTest.cpp
	tests/main_test_suite.cpp
)

# Boost library
set(Boost_USE_STATIC_LIBS ON)               # Use static libs.
add_definitions(-DBOOST_ALL_NO_LIB)         # Disable auto-linking in MSVC.
find_package(Boost 1.55.0 QUIET COMPONENTS filesystem system date_time regex unit_test_framework)

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

add_executable(tests
    	${TESTS}
)

add_dependencies(tests
    	whisperLibrary
)

target_link_libraries(tests
    	whisperLibrary
		${Boost_LIBRARIES}
)

add_definitions(-DWHISPER_BUILD)

install(TARGETS whisperLibrary RUNTIME DESTINATION .
				LIBRARY DESTINATION .)
