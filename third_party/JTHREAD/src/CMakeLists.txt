set (HEADERS jmutex.h jthread.h jmutexautolock.h ${PROJECT_BINARY_DIR}/src/jthreadconfig.h)

add_definitions(-DJTHREAD_COMPILING)

if (CMAKE_USE_WIN32_THREADS_INIT)
	set(SOURCES win32/jmutex.cpp win32/jthread.cpp)
	set(JTHREAD_CONFIG_WIN32THREADS "#define JTHREAD_CONFIG_WIN32THREADS")
	set(JTHREAD_WIN32_CRITICALSECTION OFF CACHE BOOL "If set to false, use standard mutex. If set to true, use a critical section object.")
	if (JTHREAD_WIN32_CRITICALSECTION)
		set(JTHREAD_CONFIG_JMUTEXCRITICALSECTION "#define JTHREAD_CONFIG_JMUTEXCRITICALSECTION")
	else (JTHREAD_WIN32_CRITICALSECTION)
		set(JTHREAD_CONFIG_JMUTEXCRITICALSECTION "// Using standard Win32 mutex")
	endif (JTHREAD_WIN32_CRITICALSECTION)
else (CMAKE_USE_WIN32_THREADS_INIT) # Use pthread
	set(SOURCES pthread/jmutex.cpp pthread/jthread.cpp)
	set(JTHREAD_CONFIG_WIN32THREADS "// Using pthread based threads")
	set(JTHREAD_CONFIG_JMUTEXCRITICALSECTION "")

	# Test pthread_cancel (doesn't exits on Android)
	set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
	check_cxx_source_compiles("#include <pthread.h>\nint main(void) { pthread_cancel((pthread_t)0); return 0; }" JTHREAD_HAVE_PTHREADCANCEL)
	if (NOT JTHREAD_HAVE_PTHREADCANCEL)
		#message("Enabling JTHREAD_SKIP_PTHREAD_CANCEL")
		add_definitions(-DJTHREAD_SKIP_PTHREAD_CANCEL)
	else ()
		#message("pthread_cancel appears to exist")
	endif (NOT JTHREAD_HAVE_PTHREADCANCEL)

endif (CMAKE_USE_WIN32_THREADS_INIT)

if (NOT MSVC)
	set(JTHREAD_COMPILE_STATIC_ONLY OFF CACHE BOOL "Flag indicating if only a static library should be built, or both a dynamic and static one")
else ()
	set(CMAKE_DEBUG_POSTFIX _d)
	set(JTHREAD_COMPILE_STATIC ON CACHE BOOL "Flag indicating if a static library should be built, or a dynamic one")
endif ()

if (NOT MSVC OR JTHREAD_COMPILE_STATIC)
	set(JTHREAD_INSTALLTARGETS jthread-static)
	add_library(jthread-static STATIC ${SOURCES} ${HEADERS})
	set_target_properties(jthread-static PROPERTIES OUTPUT_NAME jthread)
	set_target_properties(jthread-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	target_link_libraries(jthread-static ${CMAKE_THREAD_LIBS_INIT})
endif()

if ((NOT MSVC AND NOT JTHREAD_COMPILE_STATIC_ONLY) OR (MSVC AND NOT JTHREAD_COMPILE_STATIC))
	add_library(jthread-shared SHARED ${SOURCES} ${HEADERS})
	set_target_properties(jthread-shared PROPERTIES VERSION ${VERSION})
	set_target_properties(jthread-shared PROPERTIES OUTPUT_NAME jthread)
	set_target_properties(jthread-shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	set(JTHREAD_INSTALLTARGETS ${JTHREAD_INSTALLTARGETS} jthread-shared)
	target_link_libraries(jthread-shared ${CMAKE_THREAD_LIBS_INIT})
endif ()

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR}/src)

install(FILES ${HEADERS} DESTINATION include/jthread)
install(TARGETS ${JTHREAD_INSTALLTARGETS} DESTINATION ${LIBRARY_INSTALL_DIR})

if (NOT MSVC)
	set(JTHREAD_LIBS "-L${LIBRARY_INSTALL_DIR}" "-ljthread" ${CMAKE_THREAD_LIBS_INIT})
else ()
	set(JTHREAD_LIBS optimized "${LIBRARY_INSTALL_DIR}/jthread.lib" debug "${LIBRARY_INSTALL_DIR}/jthread_d.lib" ${CMAKE_THREAD_LIBS_INIT})
endif ()

if (NOT MSVC OR JTHREAD_COMPILE_STATIC)
	set(JTHREAD_IMPORT "")
	set(JTHREAD_EXPORT "")
else ()
	set(JTHREAD_IMPORT "__declspec(dllimport)")
	set(JTHREAD_EXPORT "__declspec(dllexport)")
endif ()

configure_file("${PROJECT_SOURCE_DIR}/src/jthreadconfig.h.in"
	       "${PROJECT_BINARY_DIR}/src/jthreadconfig.h")

foreach(ARG ${JTHREAD_LIBS})
	set(JTHREAD_LIBS_CMAKECONFIG "${JTHREAD_LIBS_CMAKECONFIG} \"${ARG}\"")
endforeach()

configure_file("${PROJECT_SOURCE_DIR}/cmake/JThreadConfig.cmake.in" 
	       "${PROJECT_BINARY_DIR}/cmake/JThreadConfig.cmake")

install(FILES 
	"${PROJECT_BINARY_DIR}/cmake/JThreadConfig.cmake" 
	DESTINATION ${LIBRARY_INSTALL_DIR}/cmake/JThread)

if (NOT MSVC)
	foreach(ARG ${JTHREAD_LIBS})
		set(JTHREAD_LIBS_PKGCONFIG "${JTHREAD_LIBS_PKGCONFIG} ${ARG}")
	endforeach()

	configure_file(${PROJECT_SOURCE_DIR}/pkgconfig/jthread.pc.in ${PROJECT_BINARY_DIR}/pkgconfig/jthread.pc)
	
	install(FILES ${PROJECT_BINARY_DIR}/pkgconfig/jthread.pc DESTINATION ${LIBRARY_INSTALL_DIR}/pkgconfig)
endif ()



