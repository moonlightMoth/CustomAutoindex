#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "dir_lister.h"


int main()
{
	dir_tree *root = list_dirs("test");

	print_tree(root, 0);
}

int print_tree(dir_tree *dt, int offset)
{
	printf("|");

	for (int i = 0; i < offset; i++)
	{
		printf("  ");
	}

	printf("%s\n", dt->name);

	for (int i = 0; i < dt->num_of_children; i++)
	{
		print_tree(dt->children[i], offset+1);
	}
}
