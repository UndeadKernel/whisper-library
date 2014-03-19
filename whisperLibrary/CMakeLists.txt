cmake_minimum_required(VERSION 2.8)

project(whisperLibrary)

set(SOURCES
	src/socketconnector.cpp
	src/channelmanager.cpp
	src/tcpheadercovertchannel.cpp
	src/tcppacket.cpp
	src/morseCoder.cpp
	src/timingcovertchannel.cpp
	src/genericpacket.cpp
	src/udppacket.cpp
	src/packetlengthcovertchannel.cpp
	src/lengthcoder.cpp
	src/udppacketgenerator.cpp
)

set(HEADERS
	include/whisperLibrary/socketconnector.hpp
	include/whisperLibrary/common.hpp
	include/whisperLibrary/channelmanager.hpp
	include/whisperLibrary/covertchannel.hpp
	include/whisperLibrary/tcpheadercovertchannel.hpp
	include/whisperLibrary/bitSetCoder.hpp
	src/tcppacket.hpp
	include/whisperLibrary/morseCoder.hpp
	include/whisperLibrary/timingcovertchannel.hpp
	include/whisperLibrary/genericpacket.hpp
	src/udppacket.hpp
	include/whisperLibrary/packetlengthcovertchannel.hpp
	include/whisperLibrary/lengthcoder.hpp
	src/udppacketgenerator.hpp
)

set(TESTS
	tests/main_test_suite.cpp
	tests/tcpPacketTest.cpp
	tests/tcpHeaderCovertChannelTest.cpp
	tests/bitSetCoderTest.cpp
	tests/morseCoderTest.cpp
	tests/timingCovertChannelTest.cpp
	tests/packetLengthCovertChannelTest.cpp
	tests/lengthCoderTest.cpp
)

# Boost library
set(Boost_USE_STATIC_LIBS ON)               # Use static libs.
add_definitions(-DBOOST_ALL_NO_LIB)         # Disable auto-linking in MSVC.
find_package(Boost 1.55.0 QUIET COMPONENTS filesystem system date_time regex unit_test_framework REQUIRED)

if (UNIX)
	find_package(Threads)
endif (UNIX)

add_definitions(-D_SCL_SECURE_NO_WARNINGS)	# disable warning C4996 in MSVC

include_directories(
		include/whisperLibrary
		${Boost_INCLUDE_DIRS}
)

add_library(whisperLibrary STATIC
    ${HEADERS}
    ${SOURCES}
)

TARGET_LINK_LIBRARIES(whisperLibrary
	${Boost_LIBRARIES}
)

add_executable(tests
    	${TESTS}
		${HEADERS}
		${SOURCES}
)

add_dependencies(tests
    	whisperLibrary
)

if (UNIX)
	target_link_libraries(
		tests
    		whisperLibrary
		${Boost_LIBRARIES}
		${CMAKE_THREAD_LIBS_INIT}
	)
else (UNIX)
	target_link_libraries(
		tests
    		whisperLibrary
		${Boost_LIBRARIES}
	)
endif (UNIX)

#add_definitions(-DWHISPER_BUILD)

install(TARGETS whisperLibrary RUNTIME DESTINATION .
				ARCHIVE DESTINATION .)
