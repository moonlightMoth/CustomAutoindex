#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "dir_lister.h"

#define MAX_OFFSET 100


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


int main()
{
	dir_tree *root = get_tree("test");

	print_tree(root, -1);
}
