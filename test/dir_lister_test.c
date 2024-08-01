#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "../src/dir_lister.h"

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		perror("Not one arg");
		return 1;
	}

	dir_tree *root = get_tree(argv[1]);

	sort_dir_tree(root);

	print_tree(root);

	destruct_dir_tree(root);
}
