#if 1
    #include <stdio.h>
#endif

#include <sys/socket.h>
#include "htmlprinter.h"

int startListening(short int port)
{
}

int main(int argc, char **args)
{
    printf("%d\n", argc);
    printf("%s\n", *args);

	int b = test(12);
	printf("%d\n", b);
}
