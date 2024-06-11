#if 1
    #include <stdio.h>
#endif

#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>

#include "htmlprinter.h"

#define HEDAER_FILE "header.html"
#define FOOTER_FILE "footer.html"
#define PRINT_V printf("%d\n", 1);

int main(int argc, char **args)
{
    int i = 0;
    long szh, szf;
    char *header_buffer, *footer_buffer;
    
    PRINT_V

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
