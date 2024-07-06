#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "dir_lister.h"

int main()
{
	dir_tree *root = get_tree("test");

	print_tree(root, -1);
}
