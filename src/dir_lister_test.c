#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "dir_lister.h"


int main()
{
	dir_tree *root = list_dirs("test");

	for (int i = 0; i < root->num_of_children; i++)
	{
		printf("%s\n", root->children[i]);
	}

}
