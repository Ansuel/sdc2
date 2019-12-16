#include "common.h"
#include "methods.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CUSTOM_ERROR(msg) do { fprintf(stderr, "%s\n", msg); exit(1); } while (0)

static void* read_pgm_line(char* input_buf, char* buf, int buf_len) {
    int i;
    for (i = 0; i < buf_len - 1; ++i) {
        buf[i] = input_buf[i];
        if (buf[i] == '\n') break;
    }
    if (++i == buf_len) {
        CUSTOM_ERROR("ERRORE: ricevuta riga di header troppo lunga!");
    }

    if (buf[0] == '#') {
        return read_pgm_line(input_buf + i, buf, buf_len); // ignora linea di commento e leggi successiva
    }

    return input_buf + i; // puntatore al primo carattere non ancora letto
}

unsigned char* parse_pgm (char* input_buf, int input_buf_len, int* p_width, int* p_height) {
    int tmp;
    char* cur_pos;

    char line_buf[8096]; // ~2K pixels per linea
    int width, height;
    unsigned short max_grey_val;

    cur_pos = read_pgm_line(input_buf, line_buf, sizeof(line_buf)); // magic number
    cur_pos = read_pgm_line(cur_pos, line_buf, sizeof(line_buf)); // width & height

    char* token = strtok(line_buf, " ");
    width = (int)strtol(token, 0, 10);
    fprintf(stderr, "Larghezza immagine: %d pixels\n", width);

    token = strtok(NULL, " ");
    height = (int)strtol(token, 0, 10);
    fprintf(stderr, "Altezza immagine: %d pixels\n", height);

    *p_width = width;
    *p_height = height;
    unsigned char *img = malloc(width*height);

    cur_pos = read_pgm_line(cur_pos, line_buf, sizeof(line_buf)); // max greyscale value
    tmp = strtol(line_buf, 0, 10);
    if (tmp > 255) {
        CUSTOM_ERROR("ERRORE: valore massimo supportato per grayscale è 255!");
    }
    max_grey_val = tmp;

    int num_pixels = width*height;
    int index = 0;
    while (index < num_pixels) {
        cur_pos = read_pgm_line(cur_pos, line_buf, sizeof(line_buf));

        token = strtok(line_buf, " ");
        tmp = strtol(token, 0, 10);
        if (tmp > max_grey_val) {
            fprintf(stderr, "ERRORE: pixel con valore %d contro un massimo atteso di %hd!", tmp, max_grey_val);
            exit(1);
        }
        img[index++] = (unsigned char)tmp;
        while ( (token = strtok(NULL, " ")) != NULL) {
            tmp = strtol(token, 0, 10);
            if (tmp > max_grey_val) {
                fprintf(stderr, "ERRORE: pixel con valore %d contro un massimo atteso di %hd!", tmp, max_grey_val);
                exit(1);
            }
            img[index++] = (unsigned char)tmp;
        }
    }

    if (cur_pos - input_buf != input_buf_len - 1) { // -1 per ultimo byte '\n' aggiunto da noi
        fprintf(stderr, "WARNING: differenza di %d bytes letti rispetto al numero atteso\n", (int)(cur_pos - input_buf));
    }

    return img;
}

int pgm_to_ascii(unsigned char* input_img, int width, int height, char** p_output_buf) {
    *p_output_buf = malloc(200+width*height*4); // fino a 200 bytes di header più 4 bytes per pixel (uno per riga)
    char* cur_pos = *p_output_buf;

    // sprintf restituisce il numero di bytes scritti (terminatore escluso)
    cur_pos += sprintf(cur_pos, "P2\n# CREATOR: demo app for SC2\n%d %d\n255\n", width, height);

    int index = 0;
    while (index < width*height) {
        // cast ad unsigned int: http://stackoverflow.com/questions/15736497/how-to-print-an-unsigned-char-in-c
        cur_pos += sprintf(cur_pos, "%u\n", (unsigned int)(input_img[index++]));
    }

    return cur_pos - *p_output_buf; // numero di bytes scritti ('\0' finale escluso)
}
