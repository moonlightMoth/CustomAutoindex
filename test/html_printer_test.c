#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include "../src/html_printer.h"

//args must be path to dir to make dir list from
//
int __check_argc(int argc)
{
	if (argc == 2)
	{
		return 0;
	}

	perror("Args count should be 1: relative or absolute path to dir to construct html of\n");

	return 1;
}

int main(int argc, char **argv)
{
	char* dwd = malloc(PATH_MAX+1);
	char* ewd = malloc(PATH_MAX+1);

	if (__check_argc(argc) == 1)
		return 1;

	if (load_wds(dwd, ewd, argv) == 1)
		return 1;

	print_html(argv[1]);

	char* buff = print_to_buffer_html_one_level(argv[1]);

	printf("%s\n", buff);

	free(buff);
	free(dwd);
	free(ewd);

}
