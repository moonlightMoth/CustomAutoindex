#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "htmlprinter.h"

int __change_path_to_exec_dir(char **args)
{
	char *dir_path = args[0], *buff;

	while (*dir_path++);
	while (*dir_path-- != '/');
	dir_path++;

	buff = (char*) malloc(dir_path-args[0]+1);

	if (buff == NULL)
	{
		perror("Got NULL in malloc __change_path_to_exec_dir");
		return 1;
	}

	memcpy(buff, args[0], dir_path-args[0]);
	buff[dir_path-args[0]] = 0;

	chdir(buff);
	free(buff);

	return 0;
}

int main(int argc, char **args)
{

	__change_path_to_exec_dir(args);
	print_html();

}
