// You should start with your previous assignment's mymalloc.c :)
//
//
#include <stdio.h> // Any other headers we need here
#include <malloc.h> // We bring in the old malloc function
		    // NOTE: You should remove malloc.h, and not include <stdlib.h> in your final implementation.


void* mymalloc(size_t s){

	void* p = (void*)malloc(s); 	// In your solution no calls to malloc should be made!
					// Determine how you will request memory :)

	if(!p){
		// We we are out of memory
		// if we get NULL back from malloc
	}
	printf("malloc %zu bytes\n",s);
	
	return p;
}

void* mycalloc(size_t nmemb, size_t s){

	void* p = (void*)calloc(nmemb,s); 	// In your solution no calls to calloc should be made!
						// Determine how you will request memory :)

	if(!p){
		// We we are out of memory
		// if we get NULL back from malloc
	}
	printf("calloc %zu bytes\n",s);
	
	return p;
}
	
void myfree(void *ptr){
	printf("Freed some memory\n");
	free(ptr);
}
	
