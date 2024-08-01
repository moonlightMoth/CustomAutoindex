#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "dir_lister.h"

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
	char c;

	if (fptr == NULL)
	{
		perror("NULL in get_file_content");
		return -1;
	}

	while ((c = getc(fptr))!= EOF)
		buffer[i++] = c;

	buffer[i] = '\0';

	return 0;
}

//write whole buffer to file

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

	fclose(optr);


	return 0;
}

char* __get_body(char* dir) //TODO UNWRAP dir_tree TO HTML BODY
{
	chdir(dest_wd);
	dir_tree *root = get_tree(dir);
	printf("%s\n", dir);
	int dirs, files;
	get_dir_tree_stat(&dirs, &files, root);

	//printf("d:%d f:%d\n", dirs, files);

	//print_tree(root);

	char* ret = malloc(strlen(root->name)+1);
	memcpy(ret, root->name, strlen(root->name)+1);
	destruct_dir_tree(root);

	chdir(exec_wd);
	return ret;
}

int print_html(char* dir)
{
	int i = 0;
    long szh, szf;
    char *header_buffer, *body_buffer, *footer_buffer, **merged;

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

    header_buffer = (char*) malloc((szh) * sizeof(char) + 1);
	body_buffer = __get_body(dir);
    footer_buffer = (char*) malloc((szf) * sizeof(char) + 1);

	merged = (char**) calloc (3, sizeof(void*));
	merged[0] = header_buffer;
	merged[1] = body_buffer;
	merged[2] = footer_buffer;

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

	__write_to_file(merged, 3);

    free(header_buffer);
	free(body_buffer);
    free(footer_buffer);
	free(merged);

}

//must be called before all operations with header


int load_wds(char* dwd, char* ewd, char** args)
{
	char *tmp, *args_ptr;
	int sz;
    if (*args[1] == '/')
    {
		tmp = args[1];
		while (*tmp++);
		while (*tmp != '/') tmp--;
		sz = tmp - args[1];

		if (sz == 0)
			sz++;

		memcpy(dwd, args[1], sz);
		dwd[sz] = '\0';
		dest_wd = dwd;
    }
    else
    {
        getcwd(dwd, PATH_MAX);
		dest_wd = dwd;
    }

	printf("%s\n", args[0]);

    if (*args[0] == '/')
    {
        tmp = args[0];
        while (*tmp++);
        sz = tmp - args[0];
        memcpy(ewd, args[0], sz);
        ewd[sz] = '\0';
		exec_wd = ewd;
    }
    else
    {
        getcwd(ewd, PATH_MAX);
        tmp = ewd;
        while (*tmp++); //move till end of str
        sz = tmp - ewd; //count size of str
        args_ptr = args[0];
        ewd[sz-1] = '/';
        while (*args_ptr) ewd[sz++] = *args_ptr++;
		sz--;
        while (ewd[sz--] != '/');
        ewd[sz+1] = '\0';
        exec_wd = ewd;
    }

	return 0;
}
