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
    char* header_buffer;

    FILE *fptr = fopen(HEDAER_FILE, "r");

    if (fptr == NULL) {
        perror("Cannot open html header file");
        return 1;
    }

    header_buffer = get_file_content(fptr, &sz);

    if (header_buffer == NULL)
    {
        perror("Cannot read html header file");
        return 1;
    }

    while (!*header_buffer)
    {
        printf("%c", *header_buffer++);
    }
    
    
    
}
