#ifndef SERVER_H_SEEN
#define SERVER_H_SEEN


#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "html_printer.h"

#define PORT 8080

int run_server()
{
	int                                server_fd, new_socket;
    ssize_t                            valread;
    struct sockaddr_in                 address;
    int                                opt = 1;
    socklen_t                          addrlen = sizeof(address);
    char                               buffer[1024] = { 0 };
    char*                              hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
        perror("socket creation failed");
        return -1;
    }

    // Forcefully attaching socket to the port 80
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	{
        perror("setsockopt");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 80
    if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) 
	{
        perror("bind failed");
        return -1;
    }
    if (listen(server_fd, 3) < 0)
	{
        perror("listen failed");
        return -1;
    }

	while (1)
	{
		if ((new_socket = accept(server_fd, (struct sockaddr*) &address, &addrlen)) < 0)
		{
        	perror("accept failed");
        	return -1;
    	}

    	valread = read(new_socket, buffer, 1024 - 1); // subtract 1 for the null

		char* uri_ptr = buffer;
		int uri_len = 0;

		while (*uri_ptr != '/' && *uri_ptr != '\0')
			uri_ptr++;

		while (*(uri_ptr + uri_len) != ' ' && *(uri_ptr + uri_len) != '\0')
			uri_len++;


		printf("%s %b %s %.*s\n", "got request from", address.sin_addr.s_addr, "with uri", uri_len, uri_ptr);

	   	send(new_socket, buffer, strlen(buffer), 0);

		close(new_socket);

	}


    return 0;
}





#endif
