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
	
	/**
     *	INSERIRE QUI IL CODICE PER RILASCIARE LE RISORSE ANCORA ALLOCATE
     *	
     *	Suggerimenti
     *	- per i semafori, usare la/e funzione/i definite in 
     *    partial_count_buffer.c
     *	- gestire eventuali errori
     */
	
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
	
	if (argc < 3)
		print_usage(argv[0]);
	char *filename = argv[1];
	int children_count = atoi(argv[2]);
	open_semaphores();
	printf("[Parent] Started, going to launch %d children\n", children_count);
	
	/**
	 *  INSERIRE QUI IL CODICE PER LANCIARE I PROCESSI FIGLIO E CREARE LE PIPE
	 *  
	 *  Suggerimenti
	 *  - vanno lanciati children_count processi figlio
	 *  - per ognuno di essi, va create una pipe in cui il padre dovrà scrivere
	 *    ed il figlio leggere (suggerimento: usare un array di appoggio)
	 *  - la variabile pipes deve puntare ad un array di interi di dimensione
	 *    children_count; essa verrà utilizzata dal processo padre per comunicare
	 *    con l'i-esimo processo figlio attraverso il descrittore in scrittura
	 *    memorizzato nella cella i-esima dell'array
	 *  - sia nel processo padre che nei figli, vanno chiusi TUTTI i descrittori
	 *    di pipe che non verrano usati
	 *  - nei processi figlio bisogna anche rilasciare un'area di memoria non
	 *    necessaria
	 *  - sempre nei processi figlio, una volta chiusi i descrittori non 
	 *    necessari e rilasciata la memoria non necessaria, bisogna invocare la
	 *    funzione child() e poi terminare il processo figlio
	 *  - la funzione child() prende due parametri: il primo è un identificatore
	 *    univoco del processo figlio (il figlio i-esimo dovrà avere i come 
	 *    identificatore), il secondo è il descrittore di lettura da pipe da 
	 *    usare per la comunicazione col processo padre
	 *  - nel processo padre, non va chiuso il semaforo write_sem
	 *  - gestire eventuali errori nelle varie chiamate
	 */
	int *pipes;
	
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
	
	/**
	 *  INSERIRE QUI IL CODICE PER CHIUDERE TUTTE LE PIPE APERTE
	 * 
	 *  Suggerimenti
	 *  - al termine, rilasciare la memoria non più necessaria
	 *  - gestire eventuali errori
	 */
	
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
	
	/**
	 *  INSERIRE QUI IL CODICE PER ATTENDERE IL TERMINE DEI PROCESSI FIGLIO E
	 *  RILASCIARE LE RISORSE RIMASTE ALLOCATE
	 * 
	 *  Suggerimenti
	 *  - usare le funzioni definite in partial_count_buffer.c
	 *  - gestire eventuali errori
	 */
	
	printf("[Parent] Completed\n");
	
	return EXIT_SUCCESS;
}
