set(SOURCES
	src/socketconnector.cpp
	src/tcppackage.cpp
)

set(HEADERS
	include/whisper-library/socketconnector.hpp
	include/whisper-library/common.hpp
	include/whisper-library/tcppackage.hpp
)

# Boost library.
set(Boost_USE_STATIC_LIBS ON)               # Use static libs.
add_definitions(-DBOOST_ALL_NO_LIB)         # Disable auto-linking in MSVC.
find_package(Boost 1.54.0 QUIET COMPONENTS filesystem system)

include_directories(
    include/whisper-library
	${Boost_INCLUDE_DIRS}
)

add_library(whisper-library SHARED
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries(whisper-library
    ${Boost_LIBRARIES}
)

if (UNIX)
	add_definitions(-DWHISPERSTATIC)
endif (UNIX)



add_definitions(-DWHISPER_BUILD)

install(TARGETS whisper-library RUNTIME DESTINATION .
				LIBRARY DESTINATION .)
