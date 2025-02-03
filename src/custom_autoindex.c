#include <sys/socket.h>
#include <stdio.h>

#include "html_printer.h"
#include "server.h"

#define HELP_STR "Usage:\n" \
                    "custom_autoindex <mode> <directory>\n" \
                    "   <directory>   root of directory to generate html from\n" \
                    "Modes:\n" \
                    "   -t, --tree     make tree with full depth starting from <directory> to file and exit\n" \
                    "   -s, --serve    start http server on port 8080 to generate html from <directory> root\n" \
                    "   -S, --single   print to stdout single html that contains <directory> content\n" //TODO make --tree to stdout

static int __check_args(int argc, char** argv)
{
    int             modes_cnt = 0;

    if (argc != 3)
    {
        return 1;
    }

    if (argv[2][0] == '-')
    {
        return 1;
    }

    if (
        strcmp(argv[1], "-t") == 0 ||
        strcmp(argv[1], "--tree") == 0 ||
        strcmp(argv[1], "-s") == 0 ||
        strcmp(argv[1], "--serve") == 0 ||
        strcmp(argv[1], "-S") == 0 ||
        strcmp(argv[1], "--single") == 0
       )
    {
        return 0;
    }
    
    return 1;
}

int main (int argc, char** argv)
{
	char          *dwd, *ewd; // destination dir and executable dir


    if (__check_args(argc, argv) != 0)
    {
        printf("%s", HELP_STR);
        return 1;
    }

	dwd = malloc(PATH_MAX+1);
    ewd = malloc(PATH_MAX+1);

    if (load_wds(dwd, ewd, argv[0], argv[2]) != 0)
	{
		perror("failed to load working dirs");
		return -1;
	}


	if (strcmp(argv[1], "-s") == 0 || strcmp(argv[1], "--serve") == 0)
	{
		if(run_server() != 0)
		{
			perror("failed socker operation");
			return -1;
		}
	}


	if (strcmp(argv[1], "-S") == 0 || strcmp(argv[1], "--single") == 0)
	{
		printf("%s\n", print_to_buffer_html_one_level(argv[2]));
	}

	if(strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "--tree") == 0)
	{
		print_html(argv[2]);
	}

	free(dwd);
	free(ewd);

	return 0;
}









