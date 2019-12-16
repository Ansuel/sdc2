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
	int ret;
	
	sem_t *sem = sem_open(sem_name, O_CREAT | O_EXCL, 0666, value);
	if (sem == SEM_FAILED && errno == EEXIST) {
		ret = sem_unlink(sem_name);
		ERROR_HELPER(ret, "Errore nella rimozione di un named semaphore");
		
		sem = sem_open(sem_name, O_CREAT | O_EXCL, 0666, value);
	}
	if (sem == SEM_FAILED)
		ERROR_HELPER(-1, "Errore nell'apertura di un named semaphore");
	
	return sem;
}

void open_semaphores(void) {
	filled_sem = open_named_semaphore(FILLED_SEM_NAME, 0);
	empty_sem = open_named_semaphore(EMPTY_SEM_NAME, BUFFER_SIZE);
	write_sem = open_named_semaphore(WRITE_SEM_NAME, 1);
	
	read_index = 0;
	
	set_write_index(0);
}

void read_partial_count(int *word_length, int *count) {
	int ret;
	
	ret = sem_wait(filled_sem);
	ERROR_HELPER(ret, "Errore nella sem wait sul semaforo filled_sem");
	
	read_partial_count_from_file(read_index, word_length, count);
	read_index = (read_index + 1) % BUFFER_SIZE;
	
	ret = sem_post(empty_sem);
	ERROR_HELPER(ret, "Errore nella sem post sul semaforo empty_sem");
}

void write_partial_count(int word_length, int count) {
	int ret;
	
	ret = sem_wait(empty_sem);
	ERROR_HELPER(ret, "Errore nella sem wait sul semaforo empty_sem");
	
	ret = sem_wait(write_sem);
	ERROR_HELPER(ret, "Errore nella sem wait sul semaforo write_sem");
	
	int write_index = get_write_index();
	write_partial_count_to_file(write_index, word_length, count);
	write_index = (write_index + 1) % BUFFER_SIZE;
	set_write_index(write_index);
	
	ret = sem_post(write_sem);
	ERROR_HELPER(ret, "Errore nella sem post sul semaforo write_sem");
	
	ret = sem_post(filled_sem);
	ERROR_HELPER(ret, "Errore nella sem post sul semaforo filled_sem");
}

void close_semaphores(void) {
	int ret;
	
	ret = sem_close(filled_sem);
	ERROR_HELPER(ret, "Errore nella chiusura del semaforo filled_sem");
	
	ret = sem_close(empty_sem);
	ERROR_HELPER(ret, "Errore nella chiusura del semaforo empty_sem");
	
	ret = sem_close(write_sem);
	ERROR_HELPER(ret, "Errore nella chiusura del semaforo write_sem");
}

void remove_semaphores(void) {
	int ret;
	
	ret = sem_unlink(FILLED_SEM_NAME);
	ERROR_HELPER(ret, "Errore nella rimozione del semaforo filled_sem");
	
	ret = sem_unlink(EMPTY_SEM_NAME);
	ERROR_HELPER(ret, "Errore nella rimozione del semaforo empty_sem");
	
	ret = sem_unlink(WRITE_SEM_NAME);
	ERROR_HELPER(ret, "Errore nella rimozione del semaforo write_sem");
}
