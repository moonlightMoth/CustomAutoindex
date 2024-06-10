#if 1
    #include <stdio.h>
#endif

#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>

#include "htmlprinter.h"

#define HEDAER_FILE "header.html"
#define FOOTER_FILE "footer.html"

int main(int argc, char **args)
{
    long sz;
    char *header_buffer, *footer_buffer;

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

    header_buffer = get_file_content(header_ptr, &sz);
    footer_buffer = get_file_content(footer_ptr, &sz);

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

    while (*header_buffer)
        printf("%c", *header_buffer++);

    printf("\n");
    while (*footer_buffer)
        printf("%c", *footer_buffer++);    
    
    
}
