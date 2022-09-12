cmake_minimum_required(VERSION 3.5)

set(CROW_VERSION_MAJOR 0)
set(CROW_VERSION_MINOR 1)
set(CROW_VERSION_PATCH 0)
set(CROW_VERSION_TRUNK false)

set(FILES 
	math.c
	core.c
	data.c
	io.c
	funcond.c
	system.c
	string.c
	)

set(HEADERS
	math.h
	core.h
	data.h
	io.h
	funcond.h
	system.h
	string.h
	)

project(crow)

if(${CROW_VERSION_TRUNK})
  set(CROW_VERSION_STRING "${CROW_VERSION_MAJOR}.${CROW_VERSION_MINOR}.${CROW_VERSION_PATCH}-trunk")
else()
  set(CROW_VERSION_STRING "${CROW_VERSION_MAJOR}.${CROW_VERSION_MINOR}.${CROW_VERSION_PATCH}")
endif()


if((${CMAKE_CXX_COMPILER_ID} MATCHES "GNU") OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -Wall -Wextra -Wpedantic -Wno-unused-parameter -g -DCROWLANG_PEDANTIC_UTF8 -DCROWLANG_USE_UTF8")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /DCROWLANG_PEDANTIC_UTF8 /DCROWLANG_USE_UTF8 /Wall")
endif()

if(DEFINED LIBCROW)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpic")
  if(${LIBCROW} STREQUAL "SHARED")
    message(STATUS "Building libcrow as a shared library")
    add_library(crow SHARED ${FILES} ${HEADERS})
  else()
    message(STATUS "Building libcrow as a static library")
    add_library(crow ${FILES} ${HEADERS})
  endif()
else()
  message(STATUS "Building Crow as an executable")
  add_executable(crow crowlang.c ${FILES} ${HEADERS})
endif()

set_target_properties(crow PROPERTIES VERSION ${CROW_VERSION_STRING} SOVERSION ${CROW_VERSION_MAJOR})
install(TARGETS crow DESTINATION bin)
