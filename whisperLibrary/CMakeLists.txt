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
	src/pcapwrapper.cpp
	src/socketSender.cpp
	src/networkconnector.cpp
	src/ipheaderv4.cpp
	src/ethernetheader.cpp
	src/tcppacketgenerator.cpp
	src/packetlengthcovertchannel.cpp
	src/lengthcoder.cpp
	src/udppacketgenerator.cpp
		src/whisper_library_wrapper.cpp
)
set(HEADERS
	include/whisperLibrary/channelmanager.hpp
	include/whisperLibrary/covertchannel.hpp
	include/whisperLibrary/tcpheadercovertchannel.hpp
	include/whisperLibrary/bitSetCoder.hpp
	src/tcppacket.hpp
	include/whisperLibrary/morseCoder.hpp
	include/whisperLibrary/timingcovertchannel.hpp
	include/whisperLibrary/genericpacket.hpp
	src/udppacket.hpp
	include/whisperLibrary/pcapwrapper.hpp
	src/socketSender.hpp
	src/rawSocketProtocol.hpp
	src/networkconnector.hpp
	src/ipheaderv4.hpp
	src/ethernetheader.hpp
	src/tcppacketgenerator.hpp
	include/whisperLibrary/packetlengthcovertchannel.hpp
	include/whisperLibrary/lengthcoder.hpp
	src/udppacketgenerator.hpp
	include/whisperLibrary/whisper_library_wrapper.hpp
)

set(TESTS
	tests/main_test_suite.cpp
	tests/tcpPacketTest.cpp
	tests/tcpHeaderCovertChannelTest.cpp
	tests/bitSetCoderTest.cpp
	tests/morseCoderTest.cpp
	tests/timingCovertChannelTest.cpp
	tests/pcapWrapperTest.cpp
	tests/socketSenderTest.cpp
	tests/networkConnectorTest.cpp
	tests/tcpPacketGeneratorTest.cpp
	tests/packetLengthCovertChannelTest.cpp
	tests/lengthCoderTest.cpp
	tests/udpPacketGeneratorTest.cpp
)

# Boost library
# Use static Libs for Win32 systems.
if (WIN32)
	set(Boost_USE_STATIC_LIBS ON)
else (WIN32)
	set(Boost_USE_STATIC_LIBS OFF)
endif (WIN32)

if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC") 
endif (UNIX)

add_definitions(-DBOOST_ALL_NO_LIB)         # Disable auto-linking in MSVC.
find_package(Boost 1.55.0 QUIET COMPONENTS filesystem system date_time regex unit_test_framework REQUIRED)

add_definitions(-D_SCL_SECURE_NO_WARNINGS)	# disable warning C4996 in MSVC

# Pcap library
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

add_executable(tests
    	${TESTS}
)

add_executable(systemtest
		tests/communicationTest.cpp
		tests/main_test_suite.cpp
)

add_library(whisperLibrary STATIC
    ${HEADERS}
    ${SOURCES}
)

TARGET_LINK_LIBRARIES(whisperLibrary
	${Boost_LIBRARIES}
	${PCAP_LIBRARIES}
)

target_link_libraries(tests
    	whisperLibrary
)

target_link_libraries(systemtest
		whisperLibrary
)

add_dependencies(tests
    	whisperLibrary
)
add_dependencies(systemtest
		whisperLibrary
)

if (UNIX)
	find_package(Threads)
	target_link_libraries(systemtest
		${CMAKE_THREAD_LIBS_INIT}
	)
	target_link_libraries(whisperLibrary
		${CMAKE_THREAD_LIBS_INIT}
	)
	target_link_libraries(tests
		${CMAKE_THREAD_LIBS_INIT}
	)	
endif(UNIX)

#ws2_32
if (WIN32)
	target_link_libraries(whisperLibrary 
		ws2_32 
		Iphlpapi
	)
endif(WIN32)
add_definitions(-DWHISPER_BUILD)

install(TARGETS whisperLibrary 
        RUNTIME DESTINATION .
		ARCHIVE DESTINATION .)
