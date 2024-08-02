#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_NAME_LEN 50
#define MAX_OFFSET 120
#define DIR_IDENTITY 0
#define FILE_IDENTITY 1
#define LINK_IDENTITY 2

#define MIN(a, b) (a) < (b) ? a : b

typedef struct dt
{
	char* name;
	int num_of_children;
	off_t size;
	time_t mtime;
	char type;
	struct dt** children;
} dir_tree;

static int __get_dir_num_of_children(char* path, struct stat *stat_buff)
{
	int num = 0;
	DIR *dp;
	struct dirent *ep;

	int err = lstat(path, stat_buff);
	if (err || S_ISLNK(stat_buff->st_mode))
	{
		return -1;
	}

	dp = opendir(path);

	if (dp == NULL)
	{
		return -1;
	}

	while (ep = readdir(dp)) num++;

	closedir(dp);

	return num - 2;
}


// fills name buffer with ..> if overflowed

static int __fill_name_buff(char* src, char* dst)
{
	if (!src || !dst)
	{
		perror("NULL one of buffers: __fill_name_buff::dir_lister.h");
		return -1;
	}

	strcpy(dst, src);

	#if 0
	int src_len = strlen(src);

	if (src_len < MAX_NAME_LEN)
	{
		memcpy(dst, src, src_len);
		dst[src_len] = '\0';
	}
	else
	{
		memcpy(dst, src, MAX_NAME_LEN-3);
		dst[MAX_NAME_LEN] = '\0';
		dst[MAX_NAME_LEN-1] = '>';
		dst[MAX_NAME_LEN-2] = '.';
		dst[MAX_NAME_LEN-3] = '.';
	}
	#endif

	return 0;
}

static int __fill_dummy(dir_tree* node, char* path, char* name, struct stat *stat_buff)
{
	int i = 0;

	if (node == NULL)
		return 1;

	__fill_name_buff(name, node->name);

	node->num_of_children = 0;
	node->children = NULL;

	lstat(path, stat_buff);
	node->size = stat_buff->st_size;
	node->mtime = stat_buff->st_mtime;
	node->type = S_ISDIR(stat_buff->st_mode) ? DIR_IDENTITY : S_ISLNK(stat_buff->st_mode) ? LINK_IDENTITY : FILE_IDENTITY;

	return 0;
}

static char* __get_entry_name (char* full_name)
{
	int entry_name_len = strlen(full_name);
	char* ret = full_name + entry_name_len - 1;

	if (entry_name_len != 1 && *ret == '/')
		*ret = '\0';

	while (ret >= full_name && *ret != '/') ret--;

	ret++;

	return ret;
}

static int __get_dir_tree_stat(int *dirs, int *files, dir_tree *node)
{
	if (node->type == FILE_IDENTITY)
		(*files)++;
	else
		(*dirs)++;

	for (int i = 0; i < node->num_of_children; i++)
	{
		__get_dir_tree_stat(dirs, files, node->children[i]);
	}
}


// takes int pointers to put stat to

int get_dir_tree_stat(int *dirs, int *files, dir_tree *root)
{
	*dirs = 0;
	*files = 0;

	__get_dir_tree_stat(dirs, files, root);
}

static dir_tree* __list_dirs(char* path, struct stat *stat_buff)
{
	dir_tree *node = (dir_tree*) malloc(sizeof(dir_tree));

	int num, i = 0, j = 0, entry_name_len;
	DIR *dp;
	struct dirent *ep;
	char* entry_name = __get_entry_name(path);
	char* end_of_path = path + strlen(path);

	entry_name_len = strlen(entry_name);
	node->name = malloc(entry_name_len+1);


	num = __get_dir_num_of_children(path, stat_buff);

	if (num == -1)
	{
		__fill_dummy(node, path, entry_name, stat_buff);
		return node;
	}

	node->type = DIR_IDENTITY;

	node->children = malloc(num*sizeof(char*));

	node->num_of_children = num;
	__fill_name_buff(entry_name, node->name);


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

			node->children[i++] = __list_dirs(path, stat_buff);

			*end_of_path = '\0';
			j = 0;
		}
	}

	closedir(dp);

	return node;
}


dir_tree* get_tree(char* path)
{
	char* buff = (char*) malloc(PATH_MAX + 1);
	buff[PATH_MAX] = '\0';
	strcpy(buff, path);

	struct stat *stat_buff = (struct stat*) malloc(sizeof(struct stat));

	dir_tree *dt = __list_dirs(buff, stat_buff);

	free(buff);
	free(stat_buff);
	return dt;
}


// private recursive fun to print dir_tree to console
static int __print_tree(dir_tree *dt, int offset)
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
		{
				int len = strlen(dt->name);
				if (len > MAX_NAME_LEN)
                	printf("%.*s..>\n", MAX_NAME_LEN-3, dt->name);
                else
                	printf("%s\n", dt->name);
		}
		else
        {
                int len = strlen(dt->name);
				if (len > MAX_NAME_LEN)
                	printf("%.*s..>", MAX_NAME_LEN-3, dt->name);
                else
                	printf("%s", dt->name);

                off += len < MAX_NAME_LEN ? len : MAX_NAME_LEN;

                while (off < MAX_OFFSET-25)
                {
                        printf(".");
                        off++;
                }

                printf("%ld", dt->size);

				int dig = 0;
				long num = dt->size;
				while (num > 0){ dig++; num = num / 10; }
				off += dig;

				while (off < MAX_OFFSET)
				{
					printf(".");
					off++;
				}

				if (dt->type == DIR_IDENTITY)
					printf("D");
				else if (dt->type == LINK_IDENTITY)
					printf(".L");
				else
					printf("F");

				printf("    %ld\n", dt->mtime);
        }

        for (int i = 0; i < dt->num_of_children; i++)
        {
                __print_tree(dt->children[i], offset+1);
        }
}

// public fun that runs recursion and prints num of dirs and files
int print_tree(dir_tree *dt)
{
	__print_tree(dt, -1);

	int dirs, files;

	get_dir_tree_stat(&dirs, &files, dt);

	printf("\n\nDirectories num: %d\nFiles num: %d\n", dirs, files);
}


int destruct_dir_tree(dir_tree *node)
{
	free(node->name);

	if (node->children)
	{
		for (int i = 0; i < node->num_of_children; i++)
		{
			destruct_dir_tree(node->children[i]);
		}
		free(node->children);
	}

	free(node);

	return 0;
}

static void __swap(dir_tree **p1, dir_tree **p2)
{
	dir_tree *tmp;

	tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;

}

static int __str_cmp(char *s1, char *s2)
{
	while (*s1 && *s2)
	{
		if (*s1 > *s2)
			return 1;
		else if (*s1 < *s2)
			return -1;
		s1++;
		s2++;
	}

	if (!*s1 && !*s2)
		return 0;

	if (!*s1 && *s2)
		return -1;

	if (*s1 && !*s2)
		return 1;

	return -2;
}

static int __sort_children(dir_tree **dt, int n)
{
	int maxIdx = 0;
	char *max = dt[0]->name;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n-i; j++)
		{
			if (__str_cmp(dt[j]->name, max) == 1)
			{
				max = dt[j]->name;
				maxIdx = j;
			}
		}
		__swap(&dt[maxIdx], &dt[n-i-1]);
		max = dt[0]->name;
		maxIdx = 0;
	}

	return 0;
}

int sort_dir_tree(dir_tree *dt)
{
	int ret;

	if (dt == NULL)
	{
		perror("Got NULL at sort_dir_tree");
		return -1;
	}

	if (dt->num_of_children == 0)
		return 0;

	if (__sort_children(dt->children, dt->num_of_children) != 0)
	{
		perror("Got -1 in sort_dir_tree from __sort_children");
		return -1;
	}

	for (int i = 0; i < dt->num_of_children; i++)
	{
		if (sort_dir_tree(dt->children[i]) != 0)
		{
			return -1;
		}
	}

	return 0;

}









