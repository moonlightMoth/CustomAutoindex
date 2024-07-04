#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_NAME_LEN 51

typedef struct dt
{
	char* name;
	int num_of_children;
	struct dt** children;
} dir_tree;

int __get_dir_len(char* root_name)
{
	int num = 0;
	DIR *dp;
	struct dirent *ep;

	dp = opendir(root_name);
	if (dp == NULL)
	{
		return -1;
	}

	while (ep = readdir(dp))
		num++;

	closedir(dp);

	return num - 2;
}

int __fill_dummy(dir_tree* node, char* name)
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

	return 0;
}

char* __get_short_name (char* full_name)
{
	int root_len = strlen(full_name);
	char* ret = full_name + root_len - 1;

	while (ret >= full_name && *ret != '/') ret--;

	ret++;

	return ret;
}

dir_tree* list_dirs(char* root_name)
{
	dir_tree *root = (dir_tree*) malloc(sizeof(dir_tree));

	int num, i, j, root_len;
	DIR *dp;
	struct dirent *ep;
	char* short_name = __get_short_name(root_name);

	root_len = strlen(short_name);
	root->name = malloc(root_len);


	num = __get_dir_len(root_name);

	if (num == -1)
	{
		__fill_dummy(root, short_name);
		return root;
	}

	root->children = malloc(num*sizeof(char*));

	root->num_of_children = num;
	memcpy(root->name, short_name, root_len);

	for (i = 0; i < num; i++)
	{
		root->children[i] = (dir_tree*) malloc(sizeof(dir_tree));
	}

	i = 0; j = 0;

	dp = opendir(root_name);

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
			root->children[i++] = list_dirs(ep->d_name);
		}
	}

	closedir(dp);

	return root;
}

