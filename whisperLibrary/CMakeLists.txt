cmake_minimum_required(VERSION 2.8)

project(whisperLibrary)

set(SOURCES
	src/channelmanager.cpp
	src/tcpheadercovertchannel.cpp
	src/tcppacket.cpp
	src/morseCoder.cpp
	src/timingcovertchannel.cpp
	src/genericpacket.cpp
	src/udppacket.cpp
	src/sniffer.cpp
	src/socketSender.cpp
)

set(HEADERS
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
	include/whisperLibrary/sniffer.hpp
	src/socketSender.hpp
	src/rawSocketProtocol.hpp
)

set(TESTS
	tests/main_test_suite.cpp
	tests/tcpPacketTest.cpp
	tests/tcpHeaderCovertChannelTest.cpp
	tests/bitSetCoderTest.cpp
	tests/morseCoderTest.cpp
	tests/timingCovertChannelTest.cpp
	tests/snifferTest.cpp
	tests/socketConnectionTest.cpp
	tests/communicationTest.cpp
)

# Boost library
set(Boost_USE_STATIC_LIBS ON)               # Use static libs.
add_definitions(-DBOOST_ALL_NO_LIB)         # Disable auto-linking in MSVC.
find_package(Boost 1.55.0 QUIET COMPONENTS filesystem system date_time regex unit_test_framework)

# Pcap library
#set(PCAP_LIB_DIR "C:/Libraries/WpdPack_4_1_2/Lib")

if(EXISTS $ENV{PCAP_ROOT})
	MESSAGE("PCAP_ROOT environment variable set ($ENV{PCAP_ROOT})")
	find_path(PCAP_INCLUDE_DIR NAMES pcap/pcap.h pcap.h PATHS $ENV{PCAP_ROOT} NO_DEFAULT_PATH PATH_SUFFIXES "Include")
	if(WIN32)
		find_library(PCAP_LIBRARY NAMES wpcap PATHS $ENV{PCAP_ROOT} NO_DEFAULT_PATH PATH_SUFFIXES "Lib")
	else(WIN32)
		find_library(PCAP_LIBRARY NAMES pcap PATHS $ENV{PCAP_ROOT})
	endif(WIN32)
else(EXISTS $ENV{PCAP_ROOT})
	MESSAGE("PCAP_ROOT environment variable set unset.")
	find_path(PCAP_INCLUDE_DIR NAMES pcap/pcap.h pcap.h PATHS ${PCAP_LIB_DIR} PATH_SUFFIXES "Include")
	if(WIN32)
		find_library(PCAP_LIBRARY NAMES wpcap PATHS ${PCAP_LIB_DIR} PATH_SUFFIXES "Lib")
	else(WIN32)
		find_library(PCAP_LIBRARY NAMES pcap PATHS ${PCAP_LIB_DIR})
	endif(WIN32)
endif(EXISTS $ENV{PCAP_ROOT})
set(PCAP_LIBRARIES ${PCAP_LIBRARY})
set(PCAP_INCLUDES ${PCAP_INCLUDE_DIR})

if(PCAP_LIBRARIES)
	MESSAGE(STATUS " Pcap Includes: ${PCAP_INCLUDES}, Lib: ${PCAP_LIBRARIES}")
else(PCAP_LIBRARIES)
	MESSAGE(FATAL " pcap library not found.")
endif(PCAP_LIBRARIES)

include_directories(
		include/whisperLibrary
		${Boost_INCLUDE_DIRS}
		${PCAP_INCLUDES}
)

add_library(whisperLibrary SHARED
    ${HEADERS}
    ${SOURCES}
)

TARGET_LINK_LIBRARIES(whisperLibrary
	${Boost_LIBRARIES}
	${PCAP_LIBRARIES}
)

add_executable(tests
    	${TESTS}
		${HEADERS}
		${SOURCES}
)

add_dependencies(tests
    	whisperLibrary
)

target_link_libraries(tests
    	whisperLibrary
		${Boost_LIBRARIES}
		${PCAP_LIBRARIES}
)

#ws2_32
if (WIN32)
	 target_link_libraries(whisperLibrary ws2_32)
	 target_link_libraries(tests ws2_32)
endif()
add_definitions(-DWHISPER_BUILD)

install(TARGETS whisperLibrary RUNTIME DESTINATION .
				LIBRARY DESTINATION .)
