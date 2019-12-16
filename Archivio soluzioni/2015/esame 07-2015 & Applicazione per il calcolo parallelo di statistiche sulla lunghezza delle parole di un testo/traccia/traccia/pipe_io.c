#include <unistd.h>

#include "common.h"
#include "methods.h"

void write_word_to_pipe(int pipe_fd, const char *word) {
	char *msg = (char*)malloc(strlen(word) + 2);
	sprintf(msg, "%s\n", word);
	
	/**
     *	INSERIRE QUI IL CODICE PER SCRIVERE LA STRINGA msg USANDO IL DESCRITTORE
     *  DI PIPE pipe_fd
     *	
     *	Suggerimenti
     *	- gestire eventuali interruzioni
     *	- gestire eventuali errori
     *  - rilasciare eventuali risorse allocate
     *	- una implementazione che si assicura che tutti i byte siano stati 
     *    inviati, ossia tenendo conto di scritture parziali, verrà premiata 
     *    nella valutazione
     */
	
}

int read_word_from_pipe(int pipe_fd, char *word) {
	
	/**
	 *	INSERIRE QUI IL CODICE PER LEGGERE UNA STRINGA, USANDO IL DESCRITTORE DI
	 *  PIPE pipe_fd, DA SCRIVERE NEL BUFFER word (DI DIMENSIONE 
	 *  MAX_WORD_LENGTH + 1), ASSICURANDOSI DI ESCLUDERE IL CARATTERE \n CHE ERA
	 *  STATO USATO PER SEGNARE IL TERMINE DELLA STRINGA NELLA FUNZIONE 
	 *  write_word_to_pipe().
	 *	RITORNARE IL NUMERO DI BYTE LETTI.
	 *  SE LA PIPE VIENE CHIUSA LATO PARENT PROCESS, RITORNARE SUBITO -1.
	 *	
	 *	Suggerimenti
	 *	- leggere un carattere per volta fino a che non si legga \n oppure si 
	 *    sia riempito il buffer
	 *	- la stringa da scrivere nel buffer deve avere il carattere \0 di 
	 *    terminazione
	 *	- gestire eventuali interruzioni
	 *	- gestire eventuali errori
	 *	- gestire la chiusura della pipe da parte del parent process
	 */	
	 
}
