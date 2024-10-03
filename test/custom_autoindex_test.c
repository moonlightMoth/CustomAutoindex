#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <threads.h>

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

int start_renew_loop(char* dir, int num_of_times)
{
    struct timespec tp;
    tp.tv_nsec = 0;

    if (num_of_times < 0)
    {
        tp.tv_sec = 60;
        while (1) {
            print_html(dir);
            thrd_sleep(&tp, 0);
        }
    }
    else
    {
        tp.tv_sec = 3;
        while (num_of_times)
        {
            print_html(dir);
            printf("Created next iteration of tree.html, %d remaining\n", num_of_times);
            thrd_sleep(&tp, 0);
            num_of_times--;
        }
    }

    return 0;

}

int main(int argc, char **argv)
{
    char* dwd = malloc(PATH_MAX+1);
    char* ewd = malloc(PATH_MAX+1);

    if (__check_argc(argc) == 1)
        return 1;

    if (load_wds(dwd, ewd, argv) == 1)
        return 1;

//TODO make socket and executors


    //start_renew_loop(argv[1], 3);

    free(dwd);
    free(ewd);

}
