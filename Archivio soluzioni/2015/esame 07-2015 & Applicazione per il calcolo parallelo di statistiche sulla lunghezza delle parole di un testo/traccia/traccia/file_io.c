#include "common.h"
#include "methods.h"

#define BUFFER_FILENAME_SUFFIX "buffer_"

char* load_from_file(const char *filename) {
	FILE *input = fopen(filename, "r");
	if (input == NULL) ERROR_HELPER(-1, "Errore nell'apertura del file in lettura");
	
	// obtain file size
	if (fseek(input , 0 , SEEK_END)) ERROR_HELPER(-1, "Errore nella fseek()");;
	long int file_size = ftell(input);
	if (file_size == -1L) ERROR_HELPER(-1, "Errore nella ftell()");
	rewind(input);
	
	// allocate buffer
	char *content = (char*)malloc(file_size + 1);
	if (content == NULL) ERROR_HELPER(-1, "Errore nell'allocazione della memoria per caricare il contenuto del file");
	
	// read file
	size_t read_bytes = fread(content, 1, file_size, input);
	if (read_bytes != file_size) ERROR_HELPER(-1, "Errore nella lettura da file");
	content[file_size] = '\0';
	
	// close file
	int ret = fclose(input);
	if (ret == EOF) ERROR_HELPER(-1, "Errore nella chiusura del file");
	
	return content;
}

void save_to_file(const char *filename, const char *content) {
	FILE *output = fopen(filename, "w");
	if (output == NULL) ERROR_HELPER(-1, "Errore nell'apertura del file in scrittura");
	
	// write to file
	size_t written_bytes =fwrite(content, 1, strlen(content), output);
	if (written_bytes != strlen(content)) ERROR_HELPER(-1, "Erore nella scrittura su file");
	
	// close file
	int ret = fclose(output);
	if (ret == EOF) ERROR_HELPER(-1, "Errore nella chiusura del file");
}

void read_partial_count_from_file(int read_index, int *word_length, int *count) {
	char filename[10];
	sprintf(filename, "%s%d", BUFFER_FILENAME_SUFFIX, read_index);
	char *content = load_from_file(filename);
	
	// word length
	char *token = strtok(content, ",");
	if (token == NULL) {
		printf("Errore nel parsing del contenuto di un buffer file (%s)\n", content);
		exit(EXIT_FAILURE);
	}
	*word_length = atoi(token);
	
	// counter
	token = strtok(NULL, ",");
	if (token == NULL) {
		printf("Errore nel parsing del contenuto di un buffer file (%s)\n", content);
		exit(EXIT_FAILURE);
	}
	*count = atoi(token);
	
	free(content);
}

void write_partial_count_to_file(int write_index, int word_length, int count) {
	char filename[20];
	sprintf(filename, "%s%d", BUFFER_FILENAME_SUFFIX, write_index);
	
	char content[20];
	sprintf(content, "%d,%d", word_length, count);
	
	save_to_file(filename, content);
}
