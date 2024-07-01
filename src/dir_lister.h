#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_NAME_LEN 51

typedef struct
{
	char* name;
	int num_of_children;
	char** children;
} dir_tree;

int __get_dir_len()
{
	
}

dir_tree* list_dirs(char* root_name)
{
	dir_tree *root = (dir_tree*) malloc(sizeof(dir_tree));

	int num = 0, i, j, root_len = strlen(root_name);
	DIR *dp;
	struct dirent *ep;

	dp = opendir(root_name);
	if (dp != NULL)
	{
		while (ep = readdir(dp))
			num++;
		closedir(dp);
	}
	

	root->num_of_children = num;
	root->children = malloc(num*sizeof(char*));
	root->name = malloc(root_len);
	memcpy(root->name, root_name, root_len);

	for (i = 0; i < num; i++)
	{
		root->children[i] = malloc(MAX_NAME_LEN);
	}

	i = 0; j = 0;

	dp = opendir(root_name);
	if (dp != NULL)
	{
		while (ep = readdir(dp))
		{
			j = 0;

			while (ep->d_name[j])
			{
				root->children[i][j] = ep->d_name[j];
				j++;
			}
			root->children[i][j] = '\0';

			i++;
		}
		closedir(dp);
	}

	return root;
}

