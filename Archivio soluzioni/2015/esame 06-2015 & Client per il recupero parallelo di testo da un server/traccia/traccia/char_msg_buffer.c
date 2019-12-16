#include <semaphore.h>

#include "common.h"
#include "methods.h"

#define CHAR_MSG_BUFFER_SIZE 5

char_msg_t* char_msg_buffer[CHAR_MSG_BUFFER_SIZE];

int read_index, write_index;

/**
 *	INSERIRE QUI IL CODICE PER LA DICHIARAZIONE DEI SEMAFORI NECESSARI PER GESTIRE IL BUFFER CIRCOLARE
 *	
 *	Suggerimenti
 *	- il buffer è puntato dalla variabile char_msg_buffer ed è allocato staticamente con dimensione CHAR_MSG_BUFFER_SIZE
 *	- il buffer deve essere gestito come un buffer circolare
 *	- i reader thread inseriscono elementi nel buffer usando la funzione put_char_msg()
 *	- il reconstructor thread leggono elementi dal buffer usando la funzione get_char_msg()
 */ 

void init_char_msg_buffer() {

		read_index = 0;
    write_index = 0;

    /**
     *	INSERIRE QUI IL CODICE PER L'INIZIALIZZAZIONE DEI SEMAFORI PRECEDENTEMENTE DICHIARATI
     *	
     *	Suggerimenti
     *	- gestire eventuali errori		 		      
     */		     

}

char_msg_t *get_char_msg() {
    
		/**
		 *	COMPLETARE IL CODICE PER GESTIRE LA LETTURA DI UN ELEMENTO DAL BUFFER
     *	
     *	Suggerimenti
     *	- gestire eventuali errori
		 */
		 
		char_msg_t *char_msg = char_msg_buffer[read_index];
    read_index = (read_index + 1) % CHAR_MSG_BUFFER_SIZE;
    
    return char_msg;
}

void put_char_msg(char_msg_t *char_msg) {
    
		/**
		 *	COMPLETARE IL CODICE PER GESTIRE L'INSERIMENTO DI UN ELEMENTO NEL BUFFER
     *	
     *	Suggerimenti
     *	- gestire eventuali errori
		 */

    char_msg_buffer[write_index] = char_msg;
    write_index = (write_index + 1) % CHAR_MSG_BUFFER_SIZE;
}

void destroy_char_msg_buffer() {
    
		
		/**
		 *	INSERIRE QUI IL CODICE PER RILASCIARE LE RISORSE ACQUISITE DURANTE L'INIZIALIZZAZIONE DEL BUFFER
     *	
     *	Suggerimenti
     *	- gestire eventuali errori
		 */
		 
}
