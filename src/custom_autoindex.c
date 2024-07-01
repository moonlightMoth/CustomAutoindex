#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include "html_printer.h"

//args must be path to dir to make dir list from
//
int __check_argc(int argc)
{
	if (argc == 2)
	{
		return 0;
	}

	perror("Args count should be 1: relative or absolute construct html of\n");

	return 1;
}

int main(int argc, char **args)
{
	char* dwd = malloc(PATH_MAX+1);
	char* ewd = malloc(PATH_MAX+1);

	if (__check_argc(argc) == 1)
		return 1;

	if (load_wds(dwd, ewd, args) == 1)
		return 1;

	print_html();

	free(dwd);
	free(ewd);

}
