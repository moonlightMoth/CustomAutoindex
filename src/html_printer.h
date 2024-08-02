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

static long __get_file_length(FILE *fptr)
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

static int __get_file_content(FILE* fptr, char* buffer)
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

static int __write_to_file(char **buff, int buff_size)
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

static long __count_body_bytes(dir_tree *dt)
{
	int files, dirs;
	get_dir_tree_stat(&files, &dirs, dt);

	return files*5120 + dirs*100 + 1;
}

static long __fill_buffer(char* buff, char* prev_path, dir_tree *node, int *offset, long *curr_pos)
{
	int i, name_len, j;

	if (node->type == DIR_IDENTITY)
	{
		//update prev_path
		int prev_path_len = strlen(prev_path);
		prev_path[prev_path_len++] = '/';
		strcpy(prev_path + prev_path_len, node->name);

		//print <details>
		for (i = 0; i < *offset; i++)
		{
			buff[*curr_pos + i] = ' ';
		}
		*curr_pos += *offset;
		memcpy(buff + *curr_pos, "<details>\n", 10);
		*curr_pos += 10;

		//increase offset
		*offset += 2;

		//print <summary>
		for (i = 0; i < *offset; i++)
		{
			buff[*curr_pos + i] = ' ';
		}
		*curr_pos += *offset;
		memcpy(buff + *curr_pos, "<summary>", 9);
		*curr_pos += 9;

		// print name to buffer. If name_len > MAX_NAME_LEN, then print only MAX_NAME_LEN bytes of name
		// and increase curr_pos by MAX_NAME_LEN. If name-len < MAX_NAME_LEN, then print full name and incr curr_pos
		name_len = strlen(node->name);
		memcpy(buff + *curr_pos, node->name, MAX_NAME_LEN);
		if (name_len > MAX_NAME_LEN)
		{
			buff[*curr_pos + MAX_NAME_LEN -1] = '>';
			buff[*curr_pos + MAX_NAME_LEN -2] = '.';
			buff[*curr_pos + MAX_NAME_LEN -3] = '.';
			buff[*curr_pos + MAX_NAME_LEN -4] = '.';
			*curr_pos += MAX_NAME_LEN;
		}
		else
		{
			*curr_pos += name_len;
		}

		//print </summary>
		memcpy(buff + *curr_pos, "</summary>\n", 11);
		*curr_pos += 11;

		//print <ul>

		for (i = 0; i < *offset; i++)
		{
			buff[*curr_pos + i] = ' ';
		}
		*curr_pos += *offset;
		memcpy(buff + *curr_pos, "<ul>\n", 5);
		*curr_pos += 5;

		//incr offset for children
		*offset += 2;

		for (i = 0; i < node->num_of_children; i++)
		{
			for (j = 0; j < *offset; j++)
			{
				buff[*curr_pos + j] = ' ';
			}
			*curr_pos += *offset;
			memcpy(buff + *curr_pos, "<li>\n", 5);
			*curr_pos += 5;

			__fill_buffer(buff, prev_path, node->children[i], offset, curr_pos);

			for (j = 0; j < *offset; j++)
			{
				buff[*curr_pos + j] = ' ';
			}
			*curr_pos += *offset;
			memcpy(buff + *curr_pos, "</li>\n", 6);
			*curr_pos += 6;
		}

		//decrease offset from children
		*offset -= 2;

		//print </ul>\n
		for (i = 0; i < *offset; i++)
		{
			buff[*curr_pos + i] = ' ';
		}
		*curr_pos += *offset;
		memcpy(buff + *curr_pos, "</ul>\n", 6);
		*curr_pos += 6;

		//print </details>\n
		for (i = 0; i < *offset; i++)
		{
			buff[*curr_pos + i] = ' ';
		}
		*curr_pos += *offset;
		memcpy(buff + *curr_pos, "</details>\n", 11);
		*curr_pos += 11;

		//delete this node from prev_path
		prev_path[prev_path_len-1] = '\0';
	}
	else
	{
		*offset += 2;

		for (i = 0; i < *offset; i++)
		{
			buff[*curr_pos + i] = ' ';
		}
		*curr_pos += *offset;
		memcpy(buff + *curr_pos, "sus\n", 4);
		*curr_pos += 4;

		*offset -= 2;
	}
}

static char* __get_body(char* dir) //TODO UNWRAP dir_tree TO HTML BODY
{
	long buff_length, curr_pos = 1;
	char *ret, *prev_path;
	int i, offset = 2;

	chdir(dest_wd);
	dir_tree *root = get_tree(dir);
	sort_dir_tree(root);
	buff_length = __count_body_bytes(root);
	ret = malloc(buff_length);
	prev_path = (char*) calloc(1, PATH_MAX+1);
	*ret = '\n';

	for (i = 0; i < root->num_of_children; i++)
	{
		__fill_buffer(ret, prev_path, root->children[i], &offset, &curr_pos);
	}

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
        while (*tmp) tmp++;
		while (*tmp != '/')
			tmp--;
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
