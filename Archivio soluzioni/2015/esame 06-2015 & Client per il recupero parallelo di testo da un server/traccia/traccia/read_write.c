#include <unistd.h>

#include "common.h"
#include "methods.h"

void send_thread_id_to_server(int server_fifo_desc, int thread_id) {
    char msg[10];
    sprintf(msg, "%d", thread_id);

    /**
     *	INSERIRE QUI IL CODICE PER SCRIVERE LA STRINGA msg USANDO IL DESCRITTORE DI FIFO server_fifo_desc,
     *	ASSICURANDOSI DI SCRIVERE ANCHE IL CARATTERE \0 DI TERMINAZIONE DELLA STRINGA
     *	
     *	Suggerimenti
     *	- gestire eventuali interruzioni
     *	- gestire eventuali errori
     *	- una implementazione che si assicura che tutti i byte siano stati inviati, ossia tenendo conto di scritture
     *    parziali, verrà premiata nella valutazione
     */		     
		
}

ssize_t read_msg_from_server(int client_fifo_desc, char *buf, ssize_t max_len) {
    
		/**
		 *	INSERIRE QUI IL CODICE PER LEGGERE UNA STRINGA, USANDO IL DESCRITTORE DI FIFO client_fifo_desc, DA SCRIVERE NEL 
		 *	BUFFER buf (DI DIMENSIONE max_len), ASSICURANDOSI DI INCLUDERE IL CARATTERE \0 DI TERMINAZIONE DELLA STRINGA.
		 *	RITORNARE IL NUMERO DI BYTE LETTI. SE LA FIFO VIENE CHIUSA LATO SERVER, RITORNARE SUBITO -1.
		 *	
		 *	Suggerimenti
		 *	- leggere un carattere per volta fino a che non si legga \0 oppure si sia riempito buf
		 *	- la stringa da scrivere in buf deve avere il carattere \0 di terminazione
		 *	- gestire eventuali interruzioni
		 *	- gestire eventuali errori
		 *	- gestire la chiusura della FIFO da parte del server come indicato sopra		 
		 *	- il numero di byte letti deve includere il carattere \0 se letto dalla FIFO		 		 		 		 
		 */		 		

}
