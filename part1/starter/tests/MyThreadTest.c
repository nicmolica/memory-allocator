#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <malloc.h>

char** greetings;

void* bonjour() {
	char* french = (char*)malloc(9 * sizeof(char));
	strcpy(french, "Bonjour!");
	greetings[0] = french;
	free(french);
	pthread_exit(NULL);
}

void* gutentag() {
	char* german = (char*)malloc(11 * sizeof(char));
        strcpy(german, "Guten tag!");
        greetings[1] = german;
	free(german);
	pthread_exit(NULL);
}

void* cefaci() {
	char* romanian = (char*)malloc(9 * sizeof(char));
        strcpy(romanian, "Ce faci!");
        greetings[2] = romanian;
	free(romanian);
	pthread_exit(NULL);
}

int main() {
	// (1) Malloc for some size of your shared data (i.e. how many strings will you have)
	greetings = (char**)malloc(3 * sizeof(char*));

	// (2) Launch some number of threads (perhaps with two or more different functions)
	pthread_t frenchid;
	pthread_t germanid;
	pthread_t romanianid;

	pthread_create(&frenchid, NULL, (void*)bonjour, NULL);
	pthread_create(&germanid, NULL, (void*)gutentag, NULL);
	pthread_create(&romanianid, NULL, (void*)cefaci, NULL);

	// (3) Join some number of threads
	pthread_join(frenchid, NULL);
	pthread_join(germanid, NULL);
	pthread_join(romanianid, NULL);
	
	// (4) Print the results of shared data (i.e. this is done sequentially)
	printf("%s\n", greetings[0]);
	printf("%s\n", greetings[1]);
	printf("%s\n", greetings[2]);
	
	// (5) Clean up your program
	free(greetings);

	return 0;
}
