#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "dir_lister.h"


int print_tree(dir_tree *dt, int offset)
{
	for (int i = 0; i < offset; i++)
	{
		printf("|  ");
	}

	if (offset != -1)
		printf("|  ");

	printf("%s\n", dt->name);

	for (int i = 0; i < dt->num_of_children; i++)
	{
		print_tree(dt->children[i], offset+1);
	}
}


int main()
{
	dir_tree *root = get_tree("test");

	print_tree(root, -1);
}
