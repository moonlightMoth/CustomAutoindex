#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "htmlprinter.h"

#define HEDAER_FILE "header.html"
#define FOOTER_FILE "footer.html"
#define PRINT_V printf("Current version is: %d\n", 1);

int __change_path_to_exec_dir(char **args)
{
	char *dir_path = args[0], *buff;

	while (*dir_path++);
	while (*dir_path-- != '/');
	dir_path++;

	buff = (char*) malloc(dir_path-args[0]+1);

	if (buff == NULL)
	{
		perror("Got NULL in malloc __change_path_to_exec_dir");
		return 1;
	}

	memcpy(buff, args[0], dir_path-args[0]);
	buff[dir_path-args[0]] = 0;

	int i= 0;

	while (buff[i])
		printf("%c", buff[i++]);

	chdir(buff);
	free(buff);

	return 0;
}

int main(int argc, char **args)
{
    int i = 0;
    long szh, szf;
    char *header_buffer, *footer_buffer, *dir_ptr = args[0];

	__change_path_to_exec_dir(args);

    FILE *header_ptr = fopen(HEDAER_FILE, "r");
    FILE *footer_ptr = fopen(FOOTER_FILE, "r");
    if (header_ptr == NULL) {
        perror("Cannot open html header file");
        return 1;
    }
    if (footer_ptr == NULL) {
        perror("Cannot open html footer file");
        return 1;
    }


    szh = get_file_length(header_ptr);
    szf = get_file_length(footer_ptr);

    header_buffer = (char*) calloc(1, (szh) * sizeof(char));
    footer_buffer = (char*) calloc(1, (szf) * sizeof(char));

    get_file_content(header_ptr, header_buffer);
    get_file_content(footer_ptr, footer_buffer);

    fclose(header_ptr);
    fclose(footer_ptr);

    if (header_buffer == NULL)
    {
        perror("Cannot read html header file");
        return 1;
    }
    if (footer_buffer == NULL)
    {
        perror("Cannot read html footer file");
        return 1;
    }    

    while (header_buffer[i])
        printf("%c", header_buffer[i++]);

    i = 0;
    printf("\n");
    while (footer_buffer[i])
        printf("%c", footer_buffer[i++]);    
    
    free(header_buffer);
    free(footer_buffer);    
}
