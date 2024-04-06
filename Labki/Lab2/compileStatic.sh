#!/bin/bash
# Create a static library and compile program with it
gcc -c hashmap.c -o hashmap.o	# compile library code
ar r lib_hashmap.a hashmap.o	# create static library
gcc -c main.c -o main.o			# compile program
gcc main.o lib_hashmap.a -o main_staticLibrary	# link library to programs code
