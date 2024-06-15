#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>


#define HEDAER_FILE "header.html"
#define FOOTER_FILE "footer.html"
#define OUT_FILE "tree.html"

char *dest_wd, *exec_wd;

long __get_file_length(FILE *fptr)
{
	long size;

	if (fptr == NULL)
	{
		perror("NULL in get_file_length");
		return -1;
	}

	if (fseek(fptr, 0, SEEK_END) < 0)
	{
		perror("Got zero size get_file_length");
		return -1;
	}

	size = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);

	return size;
}

int __get_file_content(FILE* fptr, char* buffer)
{
	int i = 0;
	char* retPtr, c;

	if (fptr == NULL)
	{
		perror("NULL in get_file_content");
		return -1;
	}

	if (retPtr == NULL)
	{
		perror("NULL in get_file_content retPtr == null");
		return -1;
	}

	while ((c = getc(fptr))!= EOF)
		buffer[i++] = c;

	return 0;
}

int __write_to_file(char **buff, int buff_size)
{
	int i;
	FILE * optr = fopen(OUT_FILE, "w");

	if (!optr)
	{
		perror("Got NULL in __write_to_file");
		return 1;
	}

	for (i = 0; i < buff_size; i++)
	{
		while (*buff[i])
		{
			putc(*buff[i]++, optr);
		}
		putc('\n', optr);
	}

	return 0;
}

int print_html()
{
	int i = 0;
    long szh, szf;
    char *header_buffer, *footer_buffer, **merged;

	chdir(exec_wd);

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


    szh = __get_file_length(header_ptr);
    szf = __get_file_length(footer_ptr);

    header_buffer = (char*) calloc(1, (szh) * sizeof(char));
    footer_buffer = (char*) calloc(1, (szf) * sizeof(char));
	merged = (char**) calloc (2, sizeof(void*));
	merged[0] = header_buffer;
	merged[1] = footer_buffer;

    __get_file_content(header_ptr, header_buffer);
    __get_file_content(footer_ptr, footer_buffer);

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

//    while (header_buffer[i])
//        printf("%c", header_buffer[i++]);

//    i = 0;
//    printf("\n");
//    while (footer_buffer[i])
//        printf("%c", footer_buffer[i++]);
	__write_to_file(merged, 2);

    free(header_buffer);
    free(footer_buffer);

}


int load_wds(char* dwd, char* ewd, char** args)
{
	char *tmp, *args_ptr;
	int sz;
    if (*args[1] == '/')
    {
        tmp = args[1];
        while (*tmp++);
        sz = tmp - args[1];
        memcpy(dwd, args[1], sz);
        dwd[sz] = 0;
		dest_wd = dwd;
    }
    else
    {
        getcwd(dwd, PATH_MAX);
        tmp = dwd;
        while (*tmp++);
        sz = tmp - dwd;
        args_ptr = args[1];
        dwd[sz-1] = '/';
        while (*args_ptr) dwd[sz++] = *args_ptr++;
        dwd[sz] = 0;
		dest_wd = dwd;
    }

    if (*args[0] == '/')
    {
        tmp = args[0];
        while (*tmp++);
        sz = tmp - args[0];
        memcpy(ewd, args[0], sz);
        ewd[sz] = 0;
		exec_wd = ewd;
    }
    else
    {
        getcwd(ewd, PATH_MAX);
        tmp = ewd;
        while (*tmp++);
        sz = tmp - ewd;
        args_ptr = args[0];
        ewd[sz-1] = '/';
        while (*args_ptr) ewd[sz++] = *args_ptr++;
        while (ewd[sz--] != '/');
        ewd[sz+1] = 0;
		exec_wd = ewd;
    }

	return 0;
}
