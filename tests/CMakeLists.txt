set(SOURCES
	bitSetCoderTest.cpp
	tcpHeaderCovertChannelTest.cpp
	main_test_suite.cpp
)

# Set include directories.
include_directories(
    ../whisperLibrary/include   
)

add_executable(tests
    ${SOURCES}
)

add_dependencies(tests
    whisperLibrary
)

# Boost library.
set(Boost_USE_STATIC_LIBS ON)               # Use static libs.
add_definitions(-DBOOST_ALL_NO_LIB)         # Disable auto-linking in MSVC.
find_package(Boost 1.55.0 QUIET COMPONENTS filesystem system unit_test_framework)


include_directories(
		include/whisperLibrary
		${Boost_INCLUDE_DIRS}
)

target_link_libraries(tests
    whisperLibrary
	${Boost_LIBRARIES}
)