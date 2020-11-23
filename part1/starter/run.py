#!/usr/bin/env python3
import os

# Compile all of our tests to make sure we
# always run the latest copy.
os.system('python compile.py')


# Run our malloc on all of the results
# print("Running Test 1 with custom allocator")
# os.system('./tests/test1_mymalloc')

# print("Running Test 2 with custom allocator")
# os.system('./tests/test2_mymalloc')

# print("Running Test 3 with custom allocator")
# os.system('./tests/test3_mymalloc')

print("Running your MyThreadTest with custom allocator")
os.system('./tests/MyThreadTest_mymalloc')
