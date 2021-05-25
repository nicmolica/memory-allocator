Nicholas Molica (nicmolica), November 2020

# C Memory Management System
This project is a small library containing manual implementations of the `malloc`, `calloc` and `free` functions in C. It uses a custom linked list to link the memory blocks together, and utilizes a best-fit model rather than first-fit model, which ensures that space isn't wasted when memory is freed and a newly allocated block is small enough to fit in that space.

## Technology Used
All code in this project is written in C, though there is a short compilation script in Python. The project makes use of mutexes to ensure that multiple threads aren't trying to allocate the same space in memory at the same time.
