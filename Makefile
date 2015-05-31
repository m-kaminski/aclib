# makefile for aclib - auto completion library
#
# Copyright (c) 2015, Maciej Kaminski.
# Published under 2-clause BSD-style license
# 1. Redistributions of source code must retain the above copyright notice
# 2. Redistributions in binary form must reproduce the above copyright notice

help: .phony
	@echo "aclib - auto completion library by Maciej Kaminski. build it yourself:"
	@echo "following make targets are possible:"
	@echo " example - example program"
	@echo " library - only library"
	@echo " all - both"
	@echo " test - execute tests (requires CMake)"
	@echo " clean - remove build products"
	@echo " rebuild - clean all test"

all: aclib.a example

library: aclib.a

example: example.c aclib.a
	gcc example.c aclib.a -g -o example

aclib.a: auto_complete.o special_keys.o vt100term.o
	ar -rcs aclib.a auto_complete.o special_keys.o vt100term.o

auto_complete.o: auto_complete.c
	gcc -c auto_complete.c -g -o auto_complete.o

vt100term.o: vt100term.c
	gcc -c vt100term.c -g -o vt100term.o

special_keys.o: special_keys.c
	gcc -c special_keys.c -g -o special_keys.o

clean: .phony
	rm -f example
	rm -f *.o
	rm -f *.a
	cd test; rm -rf CMakeCache.txt  CMakeFiles  cmake_install.cmake Makefile CTestTestfile.cmake Testing test_log

test: .phony
	rm test/test_log
	cmake test
	make -C test test
	echo "test log is contained in test/test_log"

rebuild: .phony
	make clean
	make all
	make test

# My personal make target will likely be useless for you.
package: .phony
	cd .. ; tar -cf auto_complete.tar aclib/README aclib/*.c aclib/*.h aclib/Makefile
	scp ../auto_complete.tar s2.mydevil.net:~/domains/maciejkaminski.pl/public_html

.phony:

