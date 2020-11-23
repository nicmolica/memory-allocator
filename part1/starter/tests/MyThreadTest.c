#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <malloc.h>

char** greetings;

void* bonjour(void* index) {
	int* i = (int*)index;
	char* french = (char*)malloc(9 * sizeof(char));
	strncpy(french, "Bonjour!", 9);
	greetings[*i] = french;
	pthread_exit(NULL);
}

void* gutentag(void* index) {
	int* i = (int*)index;
	char* german = (char*)malloc(11 * sizeof(char));
        strncpy(german, "Guten tag!", 11);
        greetings[*i] = german;
	pthread_exit(NULL);
}

void* cefaci(void* index) {
	int* i = (int*)index;
	char* romanian = (char*)malloc(9 * sizeof(char));
        strncpy(romanian, "Ce faci!", 9);
        greetings[*i] = romanian;
	pthread_exit(NULL);
}

int main() {
	// (1) Malloc for some size of your shared data (i.e. how many strings will you have)
	greetings = (char**)malloc(3 * sizeof(char*));

	// (2) Launch some number of threads (perhaps with two or more different functions)
	pthread_t frenchid;
	pthread_t germanid;
	pthread_t romanianid;

	int* indices = (int*)malloc(3 * sizeof(int));
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	pthread_create(&frenchid, NULL, (void*)bonjour, (void*)&indices[0]);
	pthread_create(&germanid, NULL, (void*)gutentag, (void*)&indices[1]);
	pthread_create(&romanianid, NULL, (void*)cefaci, (void*)&indices[2]);

	// (3) Join some number of threads
	pthread_join(frenchid, NULL);
	pthread_join(germanid, NULL);
	pthread_join(romanianid, NULL);
	
	// (4) Print the results of shared data (i.e. this is done sequentially)
	printf("%s\n", greetings[0]);
	printf("%s\n", greetings[1]);
	printf("%s\n", greetings[2]);
	
	// (5) Cleanup your program
	//free(greetings[0]);
	//free(greetings[1]);
	//free(greetings[2]);
	free(greetings);
	free(indices);

	return 0;
}
