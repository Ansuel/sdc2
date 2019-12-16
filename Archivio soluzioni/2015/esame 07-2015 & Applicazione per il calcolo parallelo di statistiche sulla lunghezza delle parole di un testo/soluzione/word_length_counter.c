#include <unistd.h>
#include <sys/wait.h>

#include "common.h"
#include "methods.h"

#define DELIMITERS " \t\n,.-;:_\"()–/\\?!“”"

void print_usage(const char *program_name) {
	printf("Usage: %s <input file> <children count>\n", program_name);
	exit(EXIT_FAILURE);
}

void child(int child_idx, int pipe_fd) {
	
	int ret;
	int word_length_counter[MAX_WORD_LENGTH];
	memset(word_length_counter, 0, MAX_WORD_LENGTH * sizeof(int));
	printf("[Child %d] Started\n", child_idx);
	
	// read words from pipe and update stats
	char word[MAX_WORD_LENGTH + 1];
	while (1) {
		ret = read_word_from_pipe(pipe_fd, word);
		if (ret == -1) break;
		word_length_counter[strlen(word)]++;
	}
	printf("[Child %d] Read all the words sent by the parent, start writing partial results\n", child_idx);
	
	// when the pipe is closed, write partial results to "buffer"
	int i;
	for (i = 0; i < MAX_WORD_LENGTH; i++)
		if (word_length_counter[i] > 0)
			write_partial_count(i, word_length_counter[i]);
	
	// release resources
	ret = close(pipe_fd);
	ERROR_HELPER(ret, "Errore nella chiusura della pipe in lettura nel figlio");
	
	close_semaphores();
	
	printf("[Child %d] Completed\n", child_idx);
}

void test(const int *results, const char *filename) {
	int test_results[MAX_WORD_LENGTH];
	memset(test_results, 0, MAX_WORD_LENGTH * sizeof(int));
	
	char *content = load_from_file(filename);
	char *word = strtok(content, DELIMITERS);
	while (word) {
		test_results[strlen(word)]++;
		word = strtok(NULL, DELIMITERS);
	}
	
	int i;
	int passed = 1;
	for (i = 0; i < MAX_WORD_LENGTH; i++)
		if (results[i] != test_results[i]) {
			printf("[Parent] Test Failed at word_length %d (result: %d, test: %d)\n", i, results[i], test_results[i]);
			passed = 0;
		}
	if (passed)
		printf("[Parent] TEST PASSED!!!\n");
	free(content);
}

int main(int argc, char **argv) {
	
	int ret; 
	if (argc < 3)
		print_usage(argv[0]);
	char *filename = argv[1];
	int children_count = atoi(argv[2]);
	printf("[Parent] Started, going to launch %d children\n", children_count);
	
	// prepare named semaphores for prod/cons
	open_semaphores();

	// launch children and create pipes
	int *pipes = (int*)malloc(children_count * sizeof(int));
	if (pipes == NULL)
		ERROR_HELPER(-1, "Errore nell'allocazione di memoria per i descrittori di pipe");
	int tmp[2];
	int i;
	for (i = 0; i < children_count; i++) {
		ret = pipe(tmp);
		ERROR_HELPER(ret, "Errore nella creazione di una pipe");
		
		pid_t pid = fork();
		ERROR_HELPER(pid, "Errore nella creazione di un processo figlio");
		if (pid == 0) {
			// release unused resources (pipes and memory)
			int j;
			for (j = 0; j < i; j++) {
				ret = close(pipes[j]);
				ERROR_HELPER(ret, "Errore nella chiusura di una pipe non necessaria nel figlio");
			}
			free(pipes);
			ret = close(tmp[1]);
			ERROR_HELPER(ret, "Errore nella chiusura della pipe in scrittura nel figlio");
			
			child(i, tmp[0]);
			exit(EXIT_SUCCESS);
		} else {
			ret = close(tmp[0]);
			ERROR_HELPER(ret, "Errore nella chiusura della pipe in lettura nel padre");
			pipes[i] = tmp[1];
		}
	}
	
	// parent: read input and send via pipe single words round-robin to the children
	char *input_file_content = load_from_file(filename);
	char *word = strtok(input_file_content, DELIMITERS);
	int child_idx = 0;
	int word_counter = 0;
	printf("[Parent] Going to send words read from %s to children\n", filename);
	while (word) {
		write_word_to_pipe(pipes[child_idx], word);
		child_idx = (child_idx + 1) % children_count;
		word = strtok(NULL, DELIMITERS);
		word_counter++;
	}
	printf("[Parent] Done, sent %d words\n", word_counter);
	
	// parent: at the end, close all the pipes
	for (i = 0; i < children_count; i++) {
		ret = close(pipes[i]);
		ERROR_HELPER(ret, "Errore nella chiusura di una pipe");
	}
	free(pipes);
	
	// parent: read partial results from "buffer" and update stats
	int word_length_counter[MAX_WORD_LENGTH];
	memset(word_length_counter, 0, MAX_WORD_LENGTH * sizeof(int));
	printf("[Parent] Going to read partial results written by children\n");
	while (word_counter) {
		int word_length, count;
		read_partial_count(&word_length, &count);
		word_length_counter[word_length] += count;
		word_counter -= count;
	}
	
	printf("[Parent] Done, going to test obtained results\n");	
	test(word_length_counter, filename);
	free(input_file_content);
	
	// wait for the termination of all the children
	for (i = 0; i < children_count; i++) {
		ret = wait(NULL);
		ERROR_HELPER(ret, "Errore nella wait()");
	}
	
	// release resources
	close_semaphores();
	remove_semaphores();
	
	printf("[Parent] Completed\n");
	
	return EXIT_SUCCESS;
}
