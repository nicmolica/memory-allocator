# Memory Allocator 2



# Introduction

For this assignment, you will be writing a more advanced memory allocator and implementing your own malloc, calloc, and free. The good news is you have already written one **(You may build or reuse pieces of your previous allocator assignment.)**. Now we are going to write a slightly more efficient and safe memory allocator. The goal of this is to get performance as close as possible to that of the actual malloc.

We call our allocator a *special purpose allocator* such that it will work really well for some programs, but perhaps not for all of them (for example, malloc is designed to work well for most all programs).


# Part 1 - Understand mmap and how it is different from sbrk

The first thing to know is that [brk](https://linux.die.net/man/2/sbrk) (i.e. the sbrk() function) is not the only way to request memory for our process. We previously saw how sbrk was used to extend the data segment (i.e. a place where heap memory lives). 

Unix provides another way to request memory for a process using the [mmap](https://linux.die.net/man/2/mmap) system call. mmap allows us to directly map into memory, by requesting some amount of pages of memory. This means that one constraint of mmap is that allocations with mmap must be made in multiples the size of a page (Even if you specify this incorrctly, mmap will round up to the page size). This means if our page size is 4kb (4096 bytes), and we only use 12 bytes in total during our programs execution, we have quite a bit of waste! In practice, for large desktop applications this is not a major issue.

Professor Shah and I have provided a brief set of slides here with some notes on mmap: https://docs.google.com/presentation/d/1yvQoeIJi195__KyyyN9OZEe3jf-JhccbJfNeR65A3Qc/edit?usp=sharing

## Should we use mmap or sbrk?

Because of concerns about fragmentation, [mmap](http://man7.org/linux/man-pages/man2/mmap.2.html) may not be optimal, especially if a program makes lots of little allocations. However, mmap has its advantages, it can be very fast because it maps directly to RAM. Remember, whether we are requesting memory with mmap or sbrk, at the end of the day--it is just a pointer to a block of memory.

## Task 1 - For your allocator

**WHAT YOU MUST DO**: So for our allocators, we are going to have a very simple heuristic: 
- *If the user is 'mallocing' for a page or more,  always use mmap.*
- *If it is anything less, use sbrk().*

*note*: If a user requests memory, and there is still memory available from a previous mmap--then you should use it! ***Thus, for this allocator, you need to split your blocks.***

# Part 2 - Multi-threading

In a multi-threaded environment, we cannot simply make requests to our 'malloc' and 'free' functions based on our previous implementation. We could have a scenario where two or more [threads](https://linux.die.net/man/3/pthread_create) request memory at the same time, and potentially all allocate the same block of memory in the heap. This would certainly be unlucky!

Luckily, we have [mutexes](https://linux.die.net/man/3/pthread_mutex_init) to enforce mutual exlusion and help protect against data races. Remember, when we use [pthread_mutex_lock](https://linux.die.net/man/3/pthread_mutex_lock) and [pthread_mutex_unlock](https://linux.die.net/man/3/pthread_mutex_unlock), this creates a critical section where only one thread that has acquired the lock can execute a section of code--thus enforcing sequential execution over shared data.

## Task 2 - Memory Consistency

**WHAT YOU MUST DO**: Implement  locking mechanisms such that  whenever there is an allocation (malloc or calloc) or deallocation (free) a lock protects that section of code.

## Part 3 - Efficiency with multiple free lists

Now given that we are in a multi-threaded environment, it may not be efficient to only have one 'free-list' data structure which has all of our memory. From `Task 2 - Memory Consistency`, you could simply add 1 lock to your code. So even if you have 100 cores running with 100 threads all doing allocation, they would all have to wait on a single lock. Hmm, that does not seem efficient.

So to prevent this bottleneck, we are going to have multiple lists which keep track of memory on a per process basis. This is a heuristic that will help optimize our allocations (Don't worry, at the OS level, when memory is finally handed out, that is done atomically, even if multiple threads ask for memory at the same time).

But lets think about this for a moment:

- Thread 1 and Thread 2 each request 5000 bytes through malloc.  Assuming the pagesize is 4096 bytes,
	- (mmap is called for 2 * 4096 bytes, because anything greater than a page, we round up)
	- Each Thread 1 and Thread 2 each have their own separate 'linked list' where memory is stored (i.e. your linked list that points to each block).
- Now thread 1 and thread 2 each make a second request for 2000 bytes of memory.
	- Each can concurrently traverse their own linked list to find the appropriate block.


So as each separate thread runs they are associated with locks on a per-cpu basis that the thread is running on. So, how do you figure out which process a thread is running on? Investigate [man sched_getcpu](http://man7.org/linux/man-pages/man3/sched_getcpu.3.html). Investigate [nproc](https://linux.die.net/man/1/nproc), [lscpu](https://linux.die.net/man/1/lscpu),[man get_nprocs()](https://linux.die.net/man/3/get_nprocs), and [man get_nprocs_conf()](https://www.systutorials.com/docs/linux/man/3-get_nprocs_conf/). Note, there may be additional strategies to query what cpu a thread is running on that you may use.

## Task 3 - Multiple Free Lists

**WHAT YOU MUST DO**: Have multiple lists created based on the number of cpus. Make sure to update your locking mechanism to make sure you have enough locks per cpu.

# Part 2.1 - pthreads

In this course we have been using, or looking at the pthreads library. In order to ensure you have at least one test for your allocator, you are going to write a multi-threaded program of your choosing.

## Task 4 - pthreads program

**WHAT YOU MUST DO**: Write a program called [MyThreadTest.c](./starter/tests/MyThreadTest.c) using the pthreads library that spawns at least two threads that allocate and complete a shared task. Commit this file to the 'tests' directory of this repo.

To provide some guidance for the above task, here might be an idea for a good unit test. Otherwise, I want you to think a little bit about how you would test your allocator.

```c
#include libraries here...

//
some_thread1(int index_to_shared_data){
	// Allocates some memory for an individual string
	
	// Generate some random data/string to put in shared_data
	// e.g. shared_data[index_to_shared_data] "some_thread1";
	
	// Terminate thread
}

some_thread2(int index_to_shared_data){
	// Allocates some memory for an individual string
	
	// Generate some random data/string to put in shared_data
	// e.g. shared_data[index_to_shared_data] "some_thread2";
	
	// Terminate thread
}

// Here is some data structure that will be shared.
// It is an array of strings, but nothing has been allocated yet.
char** shared_data

int main(){
	// (1) Malloc for some size of your shared data (i.e. how many strings will you have)

	// (2) Launch some number of threads (perhaps with two or more different functions)

	// (3) Join some number of threads
	
	// (4) Print the results of shared data (i.e. this is done sequentially)
	
	// (5) Cleanup your program
	// 'free shared_data'
}
```

Below are some resources on pthreads:
* (Start with this one) https://computing.llnl.gov/tutorials/pthreads/ 
* https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html

# Assignment Strategy

* You have a previous implementation of an allocator. Structurally many pieces will look the same, so you do not need to build things from scratch.
* Start with part 2 if you have no idea where to begin with part 1.
* Get started now! :)
* One way to approach the assignment
  * Start single threaded, with your code from Allocator 1  
  * Initially, ignore splitting blocks.  Use sbrk for small allocations and then mmap for large allocations.
  *  Add/Re-add support for splitting blocks from mmap and sbrk
  * Move to multiple threads, adding locks around all allocations and frees
  * Add lock per free list per cpu 


# Rubric

<table>
  <tbody>
    <tr>
      <th>Points</th>
      <th align="center">Description</th>
    </tr>
    <tr>
      <td>80% for a working Memory Allocator</td>
      <td align="left"><ul><li>Do you use mmap for large (page size or greater) allocations and sbrk() for allocations smaller than a page?<ul><li>Do you split your blocks to avoid fragmentation?</li></ul></li><li>Did you implement  locking mechanisms such that  whenever there is an allocation (malloc or calloc) or deallocation (free) a lock protects that section of code?</li><li>At a high level, I want to see you have an allocator that works on multi-threaded programs (i.e. it contains locks around critical sections), and manages multiple free lists.  Do you have multiple lists created based on the number of cpus? </li><li>You should test your allocator on a threaded application.<ul><li>It may be a good idea to test your allocator on both single and multi-threaded programs.</li><li>Including those unit tests in your repo is encouraged!</li></ul></li><li>You may use either a 'first-fit' or 'best-fit' algorithm for this allocator.</li></ul></td>
    </tr>
    <tr>
      <td>20% pthreads</td>
      <td align="left"><ul><li>Did you write a program called [MyThreadTest.c](./starter/tests/MyThreadTest.c) using the pthreads library that spawns at least two threads that allocate memory and complete a shared task. </li></ul></td>
    </tr>          
  </tbody>
</table> 
  
We are going to test this by running your allocator against malloc/calloc on a few multi-threaded programs. An example workload might be loading several images and associated text captions in memory on different threads, putting them into a buffer, and then playing them sequentially.
     
### Glossary and Tips
1. Commit your code changes to the repository often, this is best practice.
2. Do not commit your .o file or your executable file to the repository.
3. On Functionality/Style
	1. You should have comments in your code to explain what is going on, and use good style (80 characters per line maximum for example, functions that are less than 50 lines, well named variables, well named functions, etc.).

# Going Further

- Investigate the 'buddy allocator' and 'arena allocators'
- John Lakos Memory Allocator talks
	- part 1: https://www.youtube.com/watch?v=nZNd5FjSquk
	- part 2: https://www.youtube.com/watch?v=CFzuFNSpycI
- Our allocator works on the 'heap' working with dynamic memory. Other memory may work on the 'stack' which is for really *hot* code. Read up on [alloca](http://man7.org/linux/man-pages/man3/alloca.3.html) (in alloca.h) and understand that we could also write a custom stack allocator.
- Some high performance memory allocators are
	- [tcmalloc](http://goog-perftools.sourceforge.net/doc/tcmalloc.html)
	- [jemalloc](http://jemalloc.net/)
- (Pool Allocators overview) https://blog.molecular-matters.com/2012/09/17/memory-allocation-strategies-a-pool-allocator/ 	

# Feedback Loop

(An optional task that will reinforce your learning throughout the semester)

- A nice survey of dynamic memory allocation strategies: http://www.cs.northwestern.edu/~pdinda/ics-s05/doc/dsa.pdf

# F.A.Q.

- Q: Do I need to munmap the memory?
- A: No--for our allocators we will not worry about unmapping the memory. In practice our operating system will do this for us.
- Q: Can I just allocate for 10000 cpus? There is no possible way the instructors can test for that :D
- A: In the past students have done this, so we manually check if you statically allocate your lists with some fixed value. Nice try! :)
- Q: Do I need to merge blocks?
- A: It would be good to merge adjacent free blocks (right after you do a free), but is not required.
- Q: If you need to split a block but the amount of remaining memory in the block is less than the amount of memory of a new block (which we need to split the remaining memory into a new block), what should we do?
- A: In this case you do not need to do anything, that is an acceptable amount of fragmentation to live with.   

# Implementation Hints and Suggestions

1. Have one global lock for initializing your list of block_t.
2. Have a second global lock around sbrk calls, especially if you are getting errors.
3. Be conservative with your locks. That is, don't try to optimize too much by moving stuff inside and 
outside of locked sections--take the conservative approach first, then optimize later.
4. I recommend having a block_t* freelist rather than a block_t** listOfLists.
5. Make sure to have an array of pthread_mutex_lock's for each freelist that you have. The index 
of this list is the same as the list you are accessing when doing memory operations(malloc, calloc, free).
6. Make sure you have locks in  your free function and calloc as well.
7. Make sure when you memset (if you are using memset in calloc) that you are not memset'ing over your block_t.
8. When building MyThreadTest.c, if you are not sure if your test is correct, try running with with the regular malloc.
9. If your program hangs up at some point and does not continue running, it is likely a deadlock. Check the source that everytime you lock, you unlock too.

