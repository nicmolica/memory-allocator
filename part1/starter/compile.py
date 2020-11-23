#!/usr/bin/env python3
import os

# Compile all of our tests
os.system('clang -g tests/test1.c -o tests/test1')
os.system('clang -g tests/test2.c -o tests/test2')
os.system('clang -g tests/test3.c -o tests/test3')
os.system('clang -g tests/MyThreadTest.c -o tests/MyThreadTest -lpthread')



# (Optional)
# Make sure my tests do not have memory leaks
# Valgrind also has helpful information
# about how many allocs and frees took place
# os.system('valgrind ./tests/test1')
# os.system('valgrind ./tests/test2')
# os.system('valgrind ./tests/test3')
# os.system('valgrind ./tests/MyThreadTest')

# Compile our malloc program
os.system('clang -g -c mymalloc.c')


# Compile our tests with our custom allocator
os.system('clang -g -I. -o ./tests/test1_mymalloc ./tests/test1.c mymalloc.o')
os.system('clang -g -I. -o ./tests/test2_mymalloc ./tests/test2.c mymalloc.o')
os.system('clang -g -I. -o ./tests/test3_mymalloc ./tests/test3.c mymalloc.o')
os.system('clang -g -I. -o ./tests/MyThreadTest_mymalloc ./tests/MyThreadTest.c mymalloc.o -lpthread')
