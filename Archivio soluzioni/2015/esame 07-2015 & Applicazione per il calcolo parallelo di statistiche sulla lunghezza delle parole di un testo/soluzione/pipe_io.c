#include <unistd.h>

#include "common.h"
#include "methods.h"

void write_word_to_pipe(int pipe_fd, const char *word) {
	int ret;
	char *msg = (char*)malloc(strlen(word) + 2);
	sprintf(msg, "%s\n", word);
	
	size_t bytes_to_send = strlen(msg);
	size_t sent_bytes = 0;
	
	while (bytes_to_send > 0) {
		ret = write(pipe_fd, msg + sent_bytes, bytes_to_send);
		if (ret == -1 && errno == EINTR) continue;
		ERROR_HELPER(ret, "Errore nella scrittura su pipe");
		sent_bytes += ret;
		bytes_to_send -= ret;
	}
	
	free(msg);
}

int read_word_from_pipe(int pipe_fd, char *word) {
	int ret;
	char c;
	int read_bytes = 0;
	
	while(1) {
		ret = read(pipe_fd, &c, 1);
		if (ret == -1 && errno == EINTR) continue;
		ERROR_HELPER(ret, "Errore nella lettura da pipe");
		if (ret == 0) return -1;
		if (c == '\n' || read_bytes == MAX_WORD_LENGTH) { // truncate if longer than MAX_WORD_LENGTH
			word[read_bytes] = '\0';
			break;
		}
		word[read_bytes++] = c;
	}
	
	return read_bytes;
}
