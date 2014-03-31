cmake_minimum_required(VERSION 2.8)
project(whisperLibraryPidgin)

set(HEADERS
)

set(SOURCES
	whisper_library.c
)
			
if (UNIX)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
find_package(GLIB)

# libpurple plugin
find_library(LIBPURPLE_LIBRARIES purple)
find_path(LIBPURPLE_INCLUDE_DIRS purple.h PATH_SUFFIXES libpurple)
	
	
include_directories(
	${LIBPURPLE_INCLUDE_DIRS}
	${GLIB_INCLUDE_DIRS}
	../whisperLibrary/include
)
	
add_library(
	whisperLibraryPidgin SHARED
	${HEADERS}
	${SOURCES}
	${GLIB_INCLUDE_DIRS}
)

add_dependencies(whisperLibraryPidgin
	whisperLibrary
	${LIBPURPLE_LIBRARIES}
	${GLIB_LIBRARIES}
)	

target_link_libraries(
	whisperLibraryPidgin
	whisperLibrary
	${GLIB_LIBRARIES}
	${LIBPURPLE_LIBRARIES}
)


set_target_properties(whisperLibraryPidgin PROPERTIES COMPILE_FLAGS "-fPIC `pkg-config --cflags glib-2.0` `pkg-config --libs glib-2.0`")

endif (UNIX)

if (WIN32)
	#Pidgin Includes
find_path(PURPLE_INCLUDE_DIR NAMES purple.h PATH_SUFFIXES libpurple PATHS ${CMAKE_LIBRARY_PATH})
find_path(GLIB_ROOT_INCLUDE_DIR NAMES glib.mk PATH_SUFFIXES glib-2.39.0 PATHS ${CMAKE_LIBRARY_PATH})
find_path(GLIB_INCLUDE_DIR NAMES glib.h PATH_SUFFIXES glib HINTS ${GLIB_ROOT_INCLUDE_DIR})
find_path(GMODULE_INCLUDE_DIR NAMES gmodule.h PATH_SUFFIXES gmodule HINTS ${GLIB_ROOT_INCLUDE_DIR})

set(PIDGIN_INCLUDE_DIRS 
	${PURPLE_INCLUDE_DIR} 
	${GLIB_ROOT_INCLUDE_DIR} 
	${GLIB_INCLUDE_DIR}
	${GMODULE_INCLUDE_DIR}
)
#Pidgin Libs
set (PIDGIN_LIB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/lib")
find_library(PIDGIN_PIDGIN_LIBRARY 	NAMES pidgin PATHS ${PIDGIN_LIB_ROOT})
find_library(PIDGIN_INTL_LIBRARY 	NAMES intl 	 PATHS ${PIDGIN_LIB_ROOT})
find_library(PIDGIN_GLIB_LIBRARY 	NAMES libglib-2.0-0	 PATHS ${PIDGIN_LIB_ROOT})
find_library(PIDGIN_GTK_LIBRARY 	NAMES libgobject-2.0-0 PATHS ${PIDGIN_LIB_ROOT})
find_library(PIDGIN_GOBJECT_LIBRARY NAMES libgtk-win32-2.0-0 	 PATHS ${PIDGIN_LIB_ROOT})
find_library(PIDGIN_PURPLE_LIBRARY 	NAMES libpurple PATHS ${PIDGIN_LIB_ROOT})
set(PIDGIN_LIBRARIES
	${PIDGIN_PIDGIN_LIBRARY}
	${PIDGIN_INTL_LIBRARY}
	${PIDGIN_GLIB_LIBRARY}
	${PIDGIN_GTK_LIBRARY}
	${PIDGIN_GOBJECT_LIBRARY}
	${PIDGIN_PURPLE_LIBRARY}
 )
	
include_directories(
	${PIDGIN_INCLUDE_DIRS}
	../whisperLibrary/include/
)
	
add_library(whisperLibraryPidgin SHARED
	${HEADERS}
	${SOURCES}
)	


set(WHISPERSTATIC -Wl, -all -lwhisperLibrary -Wl, -notall)

add_dependencies(whisperLibraryPidgin
	${WHISPERSTATIC}
	${PIDGIN_LIBRARIES}
)

target_link_libraries(whisperLibraryPidgin
	${WHISPERSTATIC}
	${PIDGIN_LIBRARIES}
)
endif (WIN32)
install(
	TARGETS whisperLibraryPidgin
	RUNTIME DESTINATION .
	LIBRARY DESTINATION PidginPlugin
	ARCHIVE DESTINATION PidginPlugin
)

