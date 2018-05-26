cmake_minimum_required(VERSION 3.0)

project(jthread)
set(VERSION 1.3.3)

include(CheckCXXSourceCompiles)

set (_DEFAULT_LIBRARY_INSTALL_DIR lib)
if (EXISTS "${CMAKE_INSTALL_PREFIX}/lib32/" AND CMAKE_SIZEOF_VOID_P EQUAL 4)
	set (_DEFAULT_LIBRARY_INSTALL_DIR lib32)
elseif (EXISTS "${CMAKE_INSTALL_PREFIX}/lib64/" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
	set (_DEFAULT_LIBRARY_INSTALL_DIR lib64)
endif ()

set(LIBRARY_INSTALL_DIR "${_DEFAULT_LIBRARY_INSTALL_DIR}" CACHE PATH "Library installation directory")
if(NOT IS_ABSOLUTE "${LIBRARY_INSTALL_DIR}")
	set(LIBRARY_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${LIBRARY_INSTALL_DIR}")
endif()

find_package(Threads)
if (NOT CMAKE_USE_WIN32_THREADS_INIT)
	if (NOT CMAKE_USE_PTHREADS_INIT)
		message(FATAL_ERROR "Can find neither pthread support nor Win32 thread support")
	endif (NOT CMAKE_USE_PTHREADS_INIT)
endif (NOT CMAKE_USE_WIN32_THREADS_INIT)

add_subdirectory(src)

