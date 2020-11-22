#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

// Linked list structure to hold information about memory blocks.
typedef struct block {
	size_t size;
	struct block* next;
	void* memory;
	int free;
}block_t;

// A global var to easily keep track of the size of memory block headers.
#define BLOCK_SIZE sizeof(block_t)
#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

// A global var for the head of the linked list, i.e. the first memory block allocated.
block_t* head;

// Finds the best previously freed memory block to store data of a certain size in. "Best" in this sense
// means the smallest block that can fit the memory the user wants to allocate. If no such "best" block
// exists, return a NULL pointer so that the calling function knows to make a brand new block instead.
void* find_block(size_t s) {
	block_t* iter = head;
	block_t* current_best = NULL;
	while(iter != NULL) {
		if (iter->free && iter->size >= s && (current_best == NULL || iter->size - s < current_best->size - s)) {
			current_best = iter;
		}
		iter = iter->next;
	}
	return current_best;
}

// Iterate all the way down the linked list and place the new block on the end.
void insert(block_t* new_block) {
	block_t* iter = head;
	while(iter->next != NULL) {
		iter = iter->next;
	}
	iter->next = new_block;
}

// If the head hasn't been initialized, initialize it. Else, find the best block to place our new
// memory in or if no best block exists, add a new one to the end.
void* alloc(size_t s) {
	if (head == NULL) {
		head = (block_t*)sbrk(BLOCK_SIZE);
		head->size = s;
		if (s >= PAGE_SIZE) {
			head->memory = mmap(NULL, s, PROT_READ | PROT_WRITE, MAP_SHARED, -1, 0);
			if (s > PAGE_SIZE) {
                                block_t* leftovers = (block_t*)sbrk(BLOCK_SIZE);
                                leftovers->size = s % PAGE_SIZE;
                                leftovers->free = 1;
                                leftovers->memory = head->memory + s + 1;
                                insert(leftovers);
                        }
		} else {
			head->memory = sbrk(s);
		}
		head->free = 0;
		return head->memory;
	}

	block_t* new_block = find_block(s);
	if (new_block == NULL) {
		new_block = (block_t*)sbrk(BLOCK_SIZE);
		new_block->size = s;
		if (s >= PAGE_SIZE) {
                        new_block->memory = mmap(NULL, s, PROT_READ | PROT_WRITE, MAP_SHARED, -1, 0);
			if (s > PAGE_SIZE) {
				block_t* remainder = (block_t*)sbrk(BLOCK_SIZE);
				remainder->size = s % PAGE_SIZE;
				remainder->free = 1;
				remainder->memory = new_block->memory + s + 1;
				insert(remainder);
			}
		} else {
                        new_block->memory = sbrk(s);
		}
		new_block->free = 0;
		insert(new_block);
	} else {
		new_block->size = s;
		new_block->free = 0;
	}
	return new_block->memory;
}

// Call our allocation algorithm and tell the user if it fails or if it succeeds. Return a pointer
// to the beginning of the usable memory (skipping the header).
void* mymalloc(size_t s){
	void* p = alloc(s);

	if(!p) {
		fprintf(stderr, "Failed to allocate memory\n");
	} else {
		fprintf(stderr, "malloc %zu bytes\n", s);
	}

	return p;
}

// Call our allocation algorithm and then zero out the contents. Tell the user if it was successful or not.
void* mycalloc(size_t nmemb, size_t s) {
	void* p = alloc(nmemb * s);

	if(!p){
		fprintf(stderr, "Failed to allocate memory\n");
	} else {
		memset(p, 0, s * nmemb);
		fprintf(stderr, "calloc %zu bytes\n", s);
	}

	return p;
}

// Given a pointer to the beginning of a block of memory, find the associated block and mark it as
// freed. If there is no block of memory associated with the pointer passed, tell the user.
void myfree(void* ptr) {
	block_t* iter = head;
	while(iter != NULL) {
		if (iter->memory == ptr) {
			iter->free = 1;
			fprintf(stderr, "Freed some memory\n");
			return;
		} else {
			iter = iter->next;
		}
	}
	fprintf(stderr, "Failed to free memory\n");
}
