cmake_minimum_required(VERSION 3.5)

set(CROW_VERSION_MAJOR 0)
set(CROW_VERSION_MINOR 1)
set(CROW_VERSION_PATCH 0)
set(CROW_VERSION_TRUNK false)

set(FILES 
  ${CMAKE_SOURCE_DIR}/src/math.c
  ${CMAKE_SOURCE_DIR}/src/core.c
  ${CMAKE_SOURCE_DIR}/src/data.c
  ${CMAKE_SOURCE_DIR}/src/io.c
  ${CMAKE_SOURCE_DIR}/src/funcond.c
  ${CMAKE_SOURCE_DIR}/src/system.c
  ${CMAKE_SOURCE_DIR}/src/string.c
	)

set(HEADERS
	  ${CMAKE_SOURCE_DIR}/include/crow/math.h
	  ${CMAKE_SOURCE_DIR}/include/crow/core.h
	  ${CMAKE_SOURCE_DIR}/include/crow/data.h
	  ${CMAKE_SOURCE_DIR}/include/crow/io.h
	  ${CMAKE_SOURCE_DIR}/include/crow/funcond.h
	  ${CMAKE_SOURCE_DIR}/include/crow/system.h
	  ${CMAKE_SOURCE_DIR}/include/crow/string.h
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
  add_executable(crow ${CMAKE_SOURCE_DIR}/src/crowlang.c ${FILES} ${HEADERS})
endif()
target_include_directories(crow PUBLIC ${CMAKE_SOURCE_DIR}/include/)
target_link_libraries(crow m)

set_target_properties(crow PROPERTIES VERSION ${CROW_VERSION_STRING} SOVERSION ${CROW_VERSION_MAJOR})
install(TARGETS crow DESTINATION bin)
