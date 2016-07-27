CC = gcc
CXX = g++

csources = $(shell find . -name '*.c' | grep -v 'REPL\|TEST\|LIB')
cxxsources = $(shell find . -name '*.cpp' | grep -v 'REPL\|TEST\|LIB')
headers = $(shell find . -name '*.h' | grep -v 'REPL\|TEST')

sources = $(csources) $(cxxsources)

cobjects = $(csources:.c=.o)
cxxobjects = $(cxxsources:.cpp=.o)
objects = $(cobjects) $(cxxobjects)

COMMON_FLAGS = -Wall -Wextra -g -O0 -fPIC
COMMON_FLAGS += -I. -I./ast -I./front -I./runtime -I./util -I./LIB

CFLAGS = $(COMMON_FLAGS) -std=c11
CXXFLAGS = $(COMMON_FLAGS) -std=c++11

EXEFLAGS = -L. -lmoon -lm -lstdc++
LIBFLAGS = -shared -lstdc++

all : mntest mnrepl

mntest : TEST/*.c TEST/*h libmoon.a
	$(CC) -o $@ TEST/*.c $(CFLAGS) $(LDFLAGS) $(EXEFLAGS)

mnrepl : REPL/main.c libmoon.a
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS) $(EXEFLAGS)

libmoon.a : LIB/*.c LIB/*.h $(objects)
	$(CC) -o libmoon.a LIB/*.c $(LIBFLAGS) $(objects) $(CFLAGS) $(LDFLAGS)

include $(csources:.c=.d)
include $(cxxsources:.cpp=.d)

%.d: %.c
	@set -e; \
	rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.d: %.cpp
	@set -e; \
	rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY: clean

clean:
	rm -rf libmoon.a mnrepl mntest
	find . -name '*.o' -o -name '*.d*' | xargs rm -f
