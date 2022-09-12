cmake_minimum_required(VERSION 3.5)

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

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -Wall -Wextra -Wpedantic -Wno-unused-parameter -fpic -g -DCROWLANG_PEDANTIC_UTF8 -DCROWLANG_USE_UTF8")

add_executable(crow crowlang.c ${FILES} ${HEADERS})
