#!/bin/bash
# Create a dynamic library and compile program with it
gcc -c -Wall -fPIC -D_GNU_SOURCE hashmap.c	# complie library code
gcc hashmap.o -shared -o lib_hashmap.so		# create shared library
gcc -c main.c -o main.o						# complie program
gcc main.o -L. -l_hashmap -o main_dynamicLibrary	# link library
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:."	# searching for libraries