cmake_minimum_required(VERSION 3.5)

set(CROW_VERSION_MAJOR 0)
set(CROW_VERSION_MINOR 1)
set(CROW_VERSION_PATCH 0)
set(CROW_VERSION_TRUNK true)

set(FILES 
  src/math.c
  src/core.c
  src/data.c
  src/io.c
  src/funcond.c
  src/system.c
  src/string.c
	)

set(HEADERS
	  include/crow/math.h
	  include/crow/core.h
	  include/crow/data.h
	  include/crow/io.h
	  include/crow/funcond.h
	  include/crow/system.h
	  include/crow/string.h
    include/crow/types.h
	)

project(crow)

if(${CROW_VERSION_TRUNK})
  set(CROW_VERSION_STRING "${CROW_VERSION_MAJOR}.${CROW_VERSION_MINOR}.${CROW_VERSION_PATCH}-trunk")
else()
  set(CROW_VERSION_STRING "${CROW_VERSION_MAJOR}.${CROW_VERSION_MINOR}.${CROW_VERSION_PATCH}")
endif()


if((${CMAKE_CXX_COMPILER_ID} MATCHES "GNU") OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -Wall -Wextra -Wpedantic -Wno-unused-parameter -g")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Wall")
endif()

add_compile_definitions(CROWLANG_USE_UTF8)
add_compile_definitions(CROWLANG_PEDANTIC_UTF8)

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
  add_executable(crow src/crowlang.c ${FILES} ${HEADERS})
endif()

target_include_directories(crow PUBLIC include/)

if((${CMAKE_CXX_COMPILER_ID} MATCHES "GNU") OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
  target_link_libraries(crow m)
endif()

get_directory_property(submodule PARENT_DIRECTORY)
if(submodule)
    set(CROW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
endif()

set_target_properties(crow PROPERTIES VERSION ${CROW_VERSION_STRING} SOVERSION ${CROW_VERSION_MAJOR})
install(TARGETS crow DESTINATION bin)
