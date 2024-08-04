#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "dir_lister.h"

#define HEDAER_FILE "header.html"
#define FOOTER_FILE "footer.html"
#define OUT_FILE "tree.html"
#define FILE_LINE_LEN 5120
#define DIR_LINE_LEN 100

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
	int i, j = 0;
	FILE * optr = fopen(OUT_FILE, "w");

	if (!optr)
	{
		perror("Got NULL in __write_to_file");
		return 1;
	}

	for (i = 0; i < buff_size; i++)
	{
		while (buff[i][j])
		{
			putc(buff[i][j], optr);
			j++;
		}
		j = 0;
		putc('\n', optr);
	}

	fclose(optr);


	return 0;
}

static int __fill_line_with_offset(char* source, char* buff, int *offset, long *curr_pos)
{
	int i, len = strlen(source);

	for (i = 0; i < *offset; i++)
	{
		buff[*curr_pos + i] = ' ';
	}
	*curr_pos += *offset;
	memcpy(buff + *curr_pos, source, len);
	*curr_pos += len;
}

static int __fill_line(char *source, char* buff, long* curr_pos)
{
	int len = strlen(source);
	memcpy(buff + *curr_pos, source, len);
	*curr_pos += len;
}

static long __count_body_bytes(dir_tree *dt)
{
	int files, dirs;
	get_dir_tree_stat(&files, &dirs, dt);

	return files*FILE_LINE_LEN + dirs*DIR_LINE_LEN + 1;
}

static char* __get_file_line(dir_tree *node, char *prev_path)
{
	char *ret = (char*) calloc(FILE_LINE_LEN, sizeof(char));
	char *s0 = "<div class=\"hover-item\"><a href=\"https://ftp.moonlightmoth.ru/";
	char *s1 = "<div class=\"size-date\"><span>";
	char *s2 = "</span></div></div>\n";
	int i = 0, pos = 0;
	float f = 0;

	while (s0[i])
	{
		ret[pos] = s0[i];
		pos++; i++;
	}
	i = 0;

	while (prev_path[i])
	{
		ret[pos] = prev_path[i];
		pos++; i++;
	}
	i = 0;

	while (node->name[i])
	{
		ret[pos] = node->name[i];
		i++; pos++;
	}
	ret[pos++] = '\"';
	ret[pos++] = '>';
	i = 0;

	while (node->name[i])
	{
		ret[pos] = node->name[i];
		i++; pos++;
	}
	i = 0;

	ret[pos++] = '<';
	ret[pos++] = '/';
	ret[pos++] = 'a';
	ret[pos++] = '>';

	while (s1[i])
	{
		ret[pos] = s1[i];
		pos++; i++;
	}
	i = 0;

	f = (float)(node->size);

	while (f > 1023)
	{
		f /= 1024;
		i++;
	}


	sprintf(ret + pos, "%3.1f", f);
	while (ret[pos]) pos++;

	switch (i)
	{
		case 0: ret[pos++] = ' '; break;
		case 1: ret[pos++] = 'K'; break;
		case 2: ret[pos++] = 'M'; break;
		case 3: ret[pos++] = 'G'; break;
		case 4: ret[pos++] = 'T'; break;
		default: ret[pos++] = '?';
	}
	i = 0;

	ret[pos++] = 'B';
	ret[pos++] = ' ';

	strftime(ret+pos, 20, "%d.%m.%y", localtime(&(node->mtime)));
	while (ret[pos]) pos++;


	while (s2[i])
	{
		ret[pos] = s2[i];
		pos++; i++;
	}
	i = 0;

	ret[pos] = '\n';

	return ret;
}

static long __fill_buffer(char* buff, char* prev_path, dir_tree *node, int *offset, long *curr_pos)
{
	int i, name_len, j;

	if (node->type == DIR_IDENTITY)
	{
		//update prev_path
		int prev_path_len = strlen(prev_path);
		if (prev_path_len != 0)
			prev_path[prev_path_len++] = '/';
		strcpy(prev_path + prev_path_len, node->name);

		//print <details>
		__fill_line_with_offset("<details>\n", buff, offset, curr_pos);

		//increase offset
		*offset += 2;

		//print <summary>
		__fill_line_with_offset("<summary>", buff, offset, curr_pos);

		// print name to buffer. If name_len > MAX_NAME_LEN, then print only MAX_NAME_LEN bytes of name
		// and increase curr_pos by MAX_NAME_LEN. If name-len < MAX_NAME_LEN, then print full name and incr curr_pos
		name_len = strlen(node->name);
		memcpy(buff + *curr_pos, node->name, name_len+1);
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
		__fill_line("</summary>\n", buff, curr_pos);

		//print <ul>
		__fill_line_with_offset("<ul>\n", buff, offset, curr_pos);

		//incr offset for children
		*offset += 2;

		for (i = 0; i < node->num_of_children; i++)
		{
			__fill_line_with_offset("<li>\n", buff, offset, curr_pos);

			__fill_buffer(buff, prev_path, node->children[i], offset, curr_pos);

			__fill_line_with_offset("</li>\n", buff, offset, curr_pos);
		}

		//decrease offset from children
		*offset -= 2;

		//print </ul>\n
		__fill_line_with_offset("</ul>\n", buff, offset, curr_pos);

		*offset -= 2;

		//print </details>\n
		__fill_line_with_offset("</details>\n", buff, offset, curr_pos);

		//delete this node from prev_path
		if (prev_path_len != 0)
			prev_path[prev_path_len-1] = '\0';
	}
	else
	{
		*offset += 2;

		char* file_line = __get_file_line(node, prev_path);

		__fill_line_with_offset(file_line, buff, offset, curr_pos);

		free(file_line);

		*offset -= 2;
	}
}

static char* __get_body(char* dir)
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
	free(prev_path);
	ret[curr_pos] = '\0';

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
