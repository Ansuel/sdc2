#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "common.h"

void send_msg(int sock_fd, const char *msg, int msg_len) {
    /** [PUNTO 4A] COMPLETARE IL CORPO DI QUESTA FUNZIONE
     *
     * Parametri:
     * La funzione prende in input un descrittore di socket sock_fd, un
     * puntatore msg ad un buffer contenente il messaggio da inviare, e
     * la lunghezza msg_len di tale messaggio.
     *
     * Obiettivo:
     * Inviare sul descrittore il contenuto del messaggio, assicurandosi
     * che in assenza di errori *tutti* i bytes siano stati scritti.
     *
     * Note:
     * Gestire esplicitamente eventuali interruzioni ed errori.
     *
     **/
}

int recv_msg_of_given_length(int sock_fd, char* msg, int msg_len) {
    /** [PUNTO 4B] COMPLETARE IL CORPO DI QUESTA FUNZIONE
     *
     * Parametri:
     * La funzione prende in input un descrittore di socket sock_fd, un
     * puntatore msg ad un buffer ed un numero di bytes msg_len.
     *
     * Obiettivo:
     * Leggere dal descrittore il numero di bytes specificato e copiarli
     * nel buffer, assicurandosi che in assenza di errori *tutti* i bytes
     * richiesti siano stati letti. Il metodo restituisce il numero di
     * bytes letti, oppure 0 se il client chiude inaspettatamente la
     * connessione prima/durante la lettura.
     *
     * Note:
     * Gestire esplicitamente eventuali interruzioni ed errori.
     *
     **/
    int bytes_rcvd = 0;

    return bytes_rcvd;
}
