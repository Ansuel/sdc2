#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "common.h"
#include "methods.h"

/*
 * Riceve un messaggio dalla socket desiderata e lo memorizza nel
 * buffer buf di dimensione massima max_len byte.
 */
size_t recv_msg(int socket_desc, char* buf, size_t max_len) {

        /** COMPLETARE CODICE PER LA RICEZIONE DA SOCKET
     *
     * Obiettivo: scrivere nel buffer "buf" (che può contenere fino a
     *            "max_len" bytes) i byte disponibili in lettura dal
     *            descrittore di socket "socket"
     *
     * Indicazioni:
     * - se il client chiude la socket fare immediatamente "return -1"
     * - restituire il numero di byte effettivamente letti
     *
     **/

}

/*
 * Invia il messaggio contenuto nel buffer sulla socket desiderata.
 */
void send_msg(int socket, const char *msg, size_t msg_len) {

        /** COMPLETARE CODICE PER L'INVIO SU SOCKET
     *
     * Obiettivo: scrivere il contenuto del buffer "msg" sul
     *            descrittore di socket "socket"
     *
     * Suggerimento: una implementazione che si assicura che tutti i
     * msg_len byte siano stati inviati - ossia tenendo conto di invii
     * parziali - verrà premiata nella valutazione.
     *
     **/

}
