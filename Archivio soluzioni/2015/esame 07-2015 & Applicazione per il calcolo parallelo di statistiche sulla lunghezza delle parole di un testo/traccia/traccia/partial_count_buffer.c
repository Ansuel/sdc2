#include <semaphore.h>
#include <fcntl.h>

#include "common.h"
#include "methods.h"

#define BUFFER_SIZE 5
#define FILLED_SEM_NAME "/filled_sem"
#define EMPTY_SEM_NAME "/empty_sem"
#define WRITE_SEM_NAME "/write_sem"
#define WRITE_INDEX_FILENAME "write_index"

sem_t *filled_sem, *empty_sem, *write_sem;
int read_index;

int get_write_index() {
	char *content = load_from_file(WRITE_INDEX_FILENAME);
	int write_index = atoi(content);
	free(content);
	return write_index;
}

void set_write_index(int write_index) {
	char tmp[10];
	sprintf(tmp, "%d", write_index);
	save_to_file(WRITE_INDEX_FILENAME, tmp);
}

sem_t* open_named_semaphore(const char *sem_name, unsigned int value) {
	
	/**
     *	INSERIRE QUI IL CODICE PER L'APERTURA DI UN SEMAFORO
     *	
     *	Suggerimenti
     *  - il nome del semaforo è specificato nel parametro sem_name
     *  - il valore con cui inizializzare il semaforo è specificato nel parametro value
     *  - gestire l'eventualità che un semaforo con quel nome esista già: in tal caso, il semaforo va rimosso e poi ricreato
     *	- gestire eventuali errori
     *  - al termine, restituire un puntatore al semaforo
     */	
	
}

void open_semaphores(void) {
	
	/**
	 *  INSERIRE QUI IL CODICE PER INIZIALIZZARE I NAMED SEMAPHORE NECESSARI
	 *  
	 *  Suggerimenti
	 *  - le variabili da usare per i semafori sono già definite all'inizio del file
	 *  - usare la funzione open_named_semaphore()
	 */
	
	read_index = 0;
	set_write_index(0);
}

void read_partial_count(int *word_length, int *count) {
	
	/**
	 * 	COMPLETARE IL CODICE PER GESTIRE LA LETTURA DI UN COUNT PARZIALE
     *	
     *	Suggerimenti
     *	- gestire eventuali errori
	 */
		
	read_partial_count_from_file(read_index, word_length, count);
	read_index = (read_index + 1) % BUFFER_SIZE;
	
}

void write_partial_count(int word_length, int count) {
	
	/**
	 * 	COMPLETARE IL CODICE PER GESTIRE LA SCRITTURA DI UN COUNT PARZIALE
     *	
     *	Suggerimenti
     *	- gestire eventuali errori
	 */
	
	int write_index = get_write_index();
	write_partial_count_to_file(write_index, word_length, count);
	write_index = (write_index + 1) % BUFFER_SIZE;
	set_write_index(write_index);
	
}

void close_semaphores(void) {
	
	/**
	 * 	INSERIRE QUI IL CODICE PER LA CHIUSURA DEI SEMAFORI
     *	
     *	Suggerimenti
     *	- gestire eventuali errori
	 */

}

void remove_semaphores(void) {
	
	/**
	 * 	INSERIRE QUI IL CODICE PER LA RIMOZIONE DEI SEMAFORI
     *	
     *	Suggerimenti
     *	- gestire eventuali errori
	 */

}
