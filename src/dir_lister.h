#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_NAME_LEN 51
#define MAX_OFFSET 120

typedef struct dt
{
	char* name;
	int num_of_children;
	off_t size;
	struct dt** children;
} dir_tree;

int __get_dir_len(char* path)
{
	int num = 0;
	DIR *dp;
	struct dirent *ep;

	dp = opendir(path);
	if (dp == NULL)
	{
		return -1;
	}

	while (ep = readdir(dp))
		num++;

	closedir(dp);

	return num - 2;
}

int __fill_dummy(dir_tree* node, char* path, char* name, struct stat *stat_buff)
{
	int i = 0;

	if (node == NULL)
		return 1;

	while(i < MAX_NAME_LEN - 1 && name[i])
	{
		node->name[i] = name[i];
		i++;
	}

	node->name[i] = '\0';

	node->num_of_children = 0;
	node->children = NULL;

	stat(path, stat_buff);
	node->size = stat_buff->st_size;

	return 0;
}

char* __get_entry_name (char* full_name)
{
	int path_len = strlen(full_name);
	char* ret = full_name + path_len - 1;

	while (ret >= full_name && *ret != '/') ret--;

	ret++;

	return ret;
}

dir_tree* __list_dirs(char* path)
{
	dir_tree *node = (dir_tree*) malloc(sizeof(dir_tree));

	int num, i, j, path_len;
	DIR *dp;
	struct dirent *ep;
	char* entry_name = __get_entry_name(path);
	char* end_of_path = path + strlen(path);
	struct stat *stat_buff = (struct stat*) malloc(sizeof(struct stat));

	path_len = strlen(entry_name);
	node->name = malloc(path_len);


	num = __get_dir_len(path);

	if (num == -1)
	{
		__fill_dummy(node, path, entry_name, stat_buff);
		return node;
	}

	node->children = malloc(num*sizeof(char*));

	node->num_of_children = num;
	memcpy(node->name, entry_name, strlen(entry_name)+1);

	for (i = 0; i < num; i++)
	{
		node->children[i] = (dir_tree*) malloc(sizeof(dir_tree));
	}

	i = 0; j = 0;

	dp = opendir(path);

	if (dp == NULL)
	{
		perror("Got null while open dir");
		return NULL;
	}

	while (ep = readdir(dp))
	{
		if (!(
				ep->d_name[0] == '.' && ep->d_name[1] == '.' && ep->d_name[2] == '\0' ||
				ep->d_name[0] == '.' && ep->d_name[1] == '\0'
			))
		{
			*end_of_path = '/';
			while (ep->d_name[j])
			{
				end_of_path[1 + j] = ep->d_name[j];
				j++;
			}
			end_of_path[j+1] = '\0';

			node->children[i++] = __list_dirs(path);

			*end_of_path = '\0';
			j = 0;
		}
	}

	closedir(dp);
	free(stat_buff);

	return node;
}


dir_tree* get_tree(char* path)
{
	char* buff = (char*) malloc(PATH_MAX + 1);
	memcpy(buff, path, strlen(path));
	dir_tree *dt = __list_dirs(buff);
	free(buff);
	return dt;
}

int print_tree(dir_tree *dt, int offset)
{
        int off = 0;
        for (int i = 0; i < offset; i++)
        {
                printf("|  ");
                off += 3;
        }

        if (offset != -1)
        {
                printf("|  ");
                off += 3;
        }

        if (dt->children)
                printf("%s\n", dt->name);
        else
        {
                printf("%s", dt->name);
                off += strlen(dt->name);

                while (off < MAX_OFFSET)
                {
                        printf(".");
                        off++;
                }

                printf("%ld\n", dt->size);
        }

        for (int i = 0; i < dt->num_of_children; i++)
        {
                print_tree(dt->children[i], offset+1);
        }
}