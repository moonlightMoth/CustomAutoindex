#include <sys/socket.h>
#include <stdio.h>

#include "html_printer.h"

#define HELP_STR "Usage:\n" \
                    "custom_autoindex <opt> <directory>\n" \
                    "   <directory>   root of directory tree to generate html from\n" \
                    "Opt:\n" \
                    "   -t, --tree     make tree with full depth starting from <directory> to stdout and exit\n" \
                    "   -s, --serve    start http server on port 8080 to generate html from <directory> root\n" \
                    "   -S, --single   print to stdout single html that contains <directory> content\n"

int main (int argc, char** argv)
{
    printf("%s", HELP_STR);
}
