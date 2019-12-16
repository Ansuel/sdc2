#include <string.h>
#include <sys/socket.h>

#include "common.h"
#include "methods.h"

/*
 * Invia il messaggio contenuto nel buffer sulla socket desiderata.
 */
void send_msg(int socket, const char *msg) {
    int ret;

    // preparo msg_to_send copiando la stringa msg e aggiungendo '\n'
    char msg_to_send[MSG_SIZE];
    sprintf(msg_to_send, "%s\n", msg);

    /** COMPLETARE CODICE PER L'INVIO SU SOCKET
     *
     * Obiettivo: scrivere il contenuto del buffer "msg_to_send" sul
     *            descrittore di socket "socket"
     *
     * Suggerimento: una implementazione che si assicura che tutti i
     * strlen(msg_to_send) bytes siano stati inviati - ossia tenendo
     * conto di invii parziali - verrà premiata nella valutazione.
     *
     **/
}

/*
 * Riceve un messaggio dalla socket desiderata e lo memorizza nel
 * buffer buf di dimensione massima buf_len bytes.
 *
 * La fine di un messaggio in entrata è contrassegnata dal carattere
 * speciale '\n'. Il valore restituito dal metodo è il numero di byte
 * letti ('\n' escluso), o -1 nel caso in cui il client ha chiuso la
 * connessione in modo inaspettato.
 */

size_t recv_msg(int socket, char *buf, size_t buf_len) {
    int ret;
    int bytes_read = 0;

    // messaggi più lunghi di buf_len bytes vengono troncati
    while (bytes_read <= buf_len) {
        /** COMPLETARE CODICE PER LA RICEZIONE DA SOCKET
         *
         * Obiettivo: scrivere nel buffer "buf" (che può contenere fino a
         *            "buf_len" bytes) i bytes disponibili in lettura dal
         *            descrittore di socket "socket" finché non viene
         *            incontrato un terminatore di riga '\n'
         *
         * Indicazioni:
         * - leggere un byte per volta dalla socket fino al primo '\n'
         * - se il client chiude la socket fare immediatamente "return -1"
         * - quando si esce dal ciclo tramite "break", il codice che segue
         *   assume che "buf[bytes_read]" contenga '\n' e lo sovrascrive
         *   con un terminatore di stringa: il codice inserito nel ciclo
         *   dovrà quindi garantire la correttezza di questa assunzione
         *
         **/
    }

    /* Quando un messaggio viene ricevuto correttamente, il carattere
     * finale '\n' viene sostituito con un terminatore di stringa. */
    buf[bytes_read] = '\0';
    return bytes_read; // si noti che ora bytes_read == strlen(buf)
}
