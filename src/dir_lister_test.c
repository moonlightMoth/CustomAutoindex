#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "dir_lister.h"


int main()
{
	dir_tree *root = (dir_tree*) calloc(1, sizeof(dir_tree));

	int num = 0, i, j;
	DIR *dp;
	struct dirent *ep;

	dp = opendir("./test");
	if (dp != NULL)
	{
		while (ep = readdir(dp))
			num++;
		closedir(dp);
	}

	root->num_of_children = num;
	root->children = malloc(num*sizeof(char*));
	root->name = malloc(7);
	memcpy(root->name, "test/0", 5);

	for (i = 0; i < num; i++)
	{
		root->children[i] = malloc(MAX_NAME_LEN);
	}

	i = 0; j = 0;

	dp = opendir("./test");
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

	for (i = 0; i < root->num_of_children; i++)
	{
		printf("%s\n", root->children[i]);
	}

}
