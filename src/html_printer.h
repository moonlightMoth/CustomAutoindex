#ifndef HTML_PRINTER_H_SEEN
#define HTML_PRINTER_H_SEEN

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "dir_lister.h"
#include <linux/limits.h>

#define HEDAER_FILE "header.html"
#define FOOTER_FILE "footer.html"
#define OUT_FILE "tree.html"
#define FILE_LINE_LEN 5120
#define DIR_LINE_LEN 100
#define ONE_LEVEL_OFFSET 2

char *dest_wd, *exec_wd;

static long __get_file_length(FILE *fptr)
{
    long size;

    if (fptr == NULL)
    {
        perror("NULL in get_file_length\n");
        return -1;
    }

    if (fseek(fptr, 0, SEEK_END) < 0)
    {
        perror("Got zero size get_file_length\n");
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
        perror("NULL in get_file_content\n");
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
        perror("Got NULL in __write_to_file\n");
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

static int __fill_line_with_offset(char* source, char* buff, int offset, long *curr_pos)
{
    int i, len = strlen(source);

    for (i = 0; i < offset; i++)
    {
        buff[*curr_pos + i] = ' ';
    }
    *curr_pos = *curr_pos + offset;
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
    char *s0 = "<div class=\"hover-item\"><a href=\"";
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

    if (node->type == DIR_IDENTITY)
    {
        ret[pos++] = '/';
    }

    ret[pos++] = '\"';
    ret[pos++] = '>';
    i = 0;

    int name_len = strlen(node->name);

    while (node->name[i])
    {
        ret[pos + i] = node->name[i];
        i++;
    }
    
    if (name_len > MAX_NAME_LEN)
    {
        strcpy(ret+pos+MAX_NAME_LEN-3, "..>");
        pos += MAX_NAME_LEN;
    }
    else
    {
        pos += name_len;
    }

    if (node->type == DIR_IDENTITY)
    {
        ret[pos++] = '/';
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

        strcpy(prev_path + prev_path_len, node->name);
        prev_path[prev_path_len + strlen(node->name)] = '/';
        prev_path[prev_path_len + strlen(node->name) + 1] = '\0';

        //print <details>
        __fill_line_with_offset("<details>\n", buff, *offset, curr_pos);

        //increase offset
        *offset = *offset + 2;

        //print <summary>
        __fill_line_with_offset("<summary>", buff, *offset, curr_pos);

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
        __fill_line_with_offset("<ul>\n", buff, *offset, curr_pos);

        //incr offset for children
        *offset = *offset + 2;

        for (i = 0; i < node->num_of_children; i++)
        {
            __fill_line_with_offset("<li>\n", buff, *offset, curr_pos);

            __fill_buffer(buff, prev_path, node->children[i], offset, curr_pos);

            __fill_line_with_offset("</li>\n", buff, *offset, curr_pos);
        }

        //decrease offset from children
        *offset = *offset - 2;

        //print </ul>\n
        __fill_line_with_offset("</ul>\n", buff, *offset, curr_pos);

        *offset = *offset - 2;

        //print </details>\n
        __fill_line_with_offset("</details>\n", buff, *offset, curr_pos);

        //delete this node from prev_path
        prev_path[prev_path_len] = '\0';
    }
    else
    {
        *offset = *offset + 2;

        char* file_line = __get_file_line(node, prev_path);

        __fill_line_with_offset(file_line, buff, *offset, curr_pos);

        free(file_line);

        *offset = *offset - 2;
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
        perror("Cannot open html header file\n");
        return 1;
    }
    if (footer_ptr == NULL) {
        perror("Cannot open html footer file\n");
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

//must be called before all operations with header or footer
// parses zero arg to gain executable path and destination arg to gain path to start search dest dir from

int load_wds(char* dwd, char* ewd, char* exec_arg, char* dest_arg)
{
    char *tmp, *args_ptr;
    int sz;
    if (*dest_arg == '/')
    {
        tmp = dest_arg;
        while (*tmp++);
        while (*tmp != '/') tmp--;
        sz = tmp - dest_arg;

        if (sz == 0)
            sz++;

        memcpy(dwd, dest_arg, sz);
        dwd[sz] = '\0';
        dest_wd = dwd;
    }
    else
    {
        getcwd(dwd, PATH_MAX);
        dest_wd = dwd;
    }

    if (*exec_arg == '/')
    {
        tmp = exec_arg;
        while (*tmp) tmp++;
        while (*tmp != '/')
            tmp--;
        sz = tmp - exec_arg;
        memcpy(ewd, exec_arg, sz);
        ewd[sz] = '\0';
        exec_wd = ewd;
    }
    else
    {
        getcwd(ewd, PATH_MAX);
        tmp = ewd;
        while (*tmp++); //move till end of str
        sz = tmp - ewd; //count size of str
        args_ptr = exec_arg;
        ewd[sz-1] = '/';
        while (*args_ptr) ewd[sz++] = *args_ptr++;
        sz--;
        while (ewd[sz--] != '/');
        ewd[sz+1] = '\0';
        exec_wd = ewd;
    }

    return 0;
}

//----------------------------------------------------------------------------------------------------
//non-recursive workflow functions--------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

//prints non-recursive dir_tree html according to sample_non_recursive.html

static long __fill_buffer_non_recursive(char* buff, char* prev_path, dir_tree *node, long *curr_pos)
{
    int i, name_len, j;

    char* file_line = __get_file_line(node, prev_path);

    __fill_line(file_line, buff, curr_pos);

    free(file_line);
}

static char* __get_body_one_level(char* dir)
{
    long                 buff_length, curr_pos = 1;
    char                 *ret, prev_path = '\0';
    int                  i, offset = 2;
    dir_tree             *root, up_dt;

    chdir(dest_wd);

    if((root = get_non_recursive_tree(dir)) == NULL)
	{
		printf("no such directory %s\n", dir);

		return NULL;
	}

    sort_dir_tree(root);
    buff_length = __count_body_bytes(root); // too much space, can be optimized
    ret = malloc(buff_length);
    *ret = '\n';

    __fill_line_with_offset("<ul>\n", ret, offset, &curr_pos);

    up_dt.children = NULL;
    up_dt.name = "..";
    up_dt.type = DIR_IDENTITY;
    up_dt.size = 4096;
    up_dt.num_of_children = 0;
    up_dt.mtime = 0;

    __fill_line_with_offset("<li>", ret, offset, &curr_pos);
    __fill_buffer_non_recursive(ret, &prev_path, &up_dt, &curr_pos); 
    __fill_line_with_offset("</li>\n", ret, offset, &curr_pos);

    for (i = 0; i < root->num_of_children; i++)
    {
        __fill_line_with_offset("<li>", ret, offset, &curr_pos);
        __fill_buffer_non_recursive(ret, &prev_path, root->children[i], &curr_pos);
        __fill_line_with_offset("</li>\n", ret, offset, &curr_pos);
    }

    __fill_line_with_offset("</ul>\n", ret, offset, &curr_pos);

    destruct_dir_tree(root);
    ret[curr_pos] = '\0';

    chdir(exec_wd);
    return ret;
}

// merges array of chars to one buffer buff
static char* __write_to_buffer(char** merged, int cnt)
{
    int           i, lens[cnt], len;
    long          buff_len = 0, curr_pos = 0;
    char*         ret, ptr;

    for (i = 0; i < cnt; i++)
    {
        len = strlen(merged[i]);
        buff_len += len;
        lens[i] = len;
    }

    ret = (char*) malloc(buff_len * sizeof(char) + 1);

    for (i = 0; i < cnt; i++)
    {
        strcpy(ret + curr_pos, merged[i]);
        curr_pos += lens[i];
    }

    ret[curr_pos] = '\0';
    
    return ret;
}

// returns full one level html as buffer
char* print_to_buffer_html_one_level(char* dir)
{
    int           i = 0;
    long          szh, szf;
    char          *header_buffer, *body_buffer, *footer_buffer, **merged, *ret;

    chdir(exec_wd);

    FILE *header_ptr = fopen(HEDAER_FILE, "r");
    FILE *footer_ptr = fopen(FOOTER_FILE, "r");

    if (header_ptr == NULL) {
        perror("Cannot open html header file\n");

		fclose(header_ptr);
		fclose(footer_ptr);

        return NULL;
    }
    if (footer_ptr == NULL) {
        perror("Cannot open html footer file\n");

		fclose(header_ptr);
		fclose(footer_ptr);

        return NULL;
    }


    szh = __get_file_length(header_ptr);
    szf = __get_file_length(footer_ptr);

    header_buffer = (char*) malloc((szh) * sizeof(char) + 1);

	if((body_buffer = __get_body_one_level(dir)) == NULL)
	{
		printf("cannot make html from %s\n", dir);

	    free(header_buffer);
		fclose(header_ptr);
		fclose(footer_ptr);

		return NULL;
	}

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
        perror("Cannot read html header file\n");

   		free(header_buffer);
   		free(body_buffer);
   		free(footer_buffer);
	    free(merged);
		fclose(header_ptr);
		fclose(footer_ptr);


        return NULL;
    }
    if (footer_buffer == NULL)
    {
        perror("Cannot read html footer file\n");

   		free(header_buffer);
   		free(body_buffer);
   		free(footer_buffer);
	    free(merged);
		fclose(header_ptr);
		fclose(footer_ptr);


        return NULL;
    }

    ret = __write_to_buffer(merged, 3);

    free(header_buffer);
    free(body_buffer);
    free(footer_buffer);
    free(merged);

    return ret;

}

//prints non-recursive dir_tree html according to sample_non_recursive.html to file

int print_html_one_level(char* dir)
{
    int i = 0;
    long szh, szf;
    char *header_buffer, *body_buffer, *footer_buffer, **merged;

    chdir(exec_wd);

    FILE *header_ptr = fopen(HEDAER_FILE, "r");
    FILE *footer_ptr = fopen(FOOTER_FILE, "r");

    if (header_ptr == NULL) {
        perror("Cannot open html header file\n");
        return 1;
    }
    if (footer_ptr == NULL) {
        perror("Cannot open html footer file\n");
        return 1;
    }


    szh = __get_file_length(header_ptr);
    szf = __get_file_length(footer_ptr);

    header_buffer = (char*) malloc((szh) * sizeof(char) + 1);
    body_buffer = __get_body_one_level(dir);
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
        perror("Cannot read html header file\n");
        return 1;
    }
    if (footer_buffer == NULL)
    {
        perror("Cannot read html footer file\n");
        return 1;
    }

    __write_to_file(merged, 3);

    free(header_buffer);
    free(body_buffer);
    free(footer_buffer);
    free(merged);

}







#endif
