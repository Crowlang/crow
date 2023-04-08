# Crowlang Makefile for UNIX and UNIX-like systems
# This Makefile assumes GNU Make, and also GCC or a GCC compatible compiler

CFLAGS += -std=c89 -Wall -Wextra -Wpedantic -Wno-unused-parameter -fpic -g
CFLAGS += -I include/
CFLAGS += -DCROWLANG_PEDANTIC_UTF8 -DCROWLANG_USE_UTF8

# General optimizations for release builds
CFLAGS += -O2

# Only enable these if you are building for your system and only your system
# also only if you are using gcc
#CFLAGS += -march=native

# If you use any of these options, expect errors and warnings
#CFLAGS += -DCROWLANG_GC_OLD
#CFLAGS += -DCROWLANG_DISABLE_GC
#CFLAGS += -DCROWLANG_GC_DEBUG
#CFLAGS += -DCROWLANG_ALLOC_DEBUG
#CFLAGS += -DCROWLANG_SCOPE_DEBUG

# Don't edit anything under this line unless you know what you're doing

OBJS =
OBJS += math.o
OBJS += core.o
OBJS += data.o
OBJS += io.o
OBJS += funcond.o
OBJS += system.o
OBJS += string.o

crow: crowlang.o $(OBJS)
	$(CC) *.o -lm -o crow

libcrow.a: $(OBJS)
	ar -crs libcrow.a $(OBJS)

libcrow.so: $(OBJS)
	$(CC) -fpic -shared -o libcrow.so *.o

%.o: src/%.c
	$(CC) -c $(CFLAGS) src/$(@:.o=.c) -o $@

.PHONY: clean
clean:
	rm -f *.o crow libcrow.a libcrow.so
