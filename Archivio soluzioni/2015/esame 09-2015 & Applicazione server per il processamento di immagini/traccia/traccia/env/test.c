#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void *foo(void *args) {
	printf("I'm the thread!!!\n");
	pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
	
	printf("Creating a thread...\n");
	pthread_t thread;
	int ret = pthread_create(&thread, NULL, foo, NULL);
	if (ret) {
		printf("Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	printf("Thread created, waiting for its termination...\n");
	ret = pthread_join(thread, NULL);
	if (ret) {
		printf("Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	printf("Thread terminated\n");
	exit(EXIT_SUCCESS);
}
