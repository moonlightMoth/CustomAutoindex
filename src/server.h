#ifndef SERVER_H_SEEN
#define SERVER_H_SEEN


#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "html_printer.h"

#define PORT 8080
#define BUFF_SIZE 4096
#define FORBIDDEN_NO_BACKWARDS "HTTP/1.1 403 Forbidden\nContent-Type: application/json\n\n{\n  \"error\": \"Wrong request\",\n  \"message\": \"/../ not allowed in request\"\n}\n"
#define FORBIDDEN_ONLY_GET "HTTP/1.1 403 Forbidden\nContent-Type: application/json\n\n{\n  \"error\": \"Wrong request\",\n  \"message\": \"only GET allowed\"\n}\n"


static char* __get_html_buffer_by_uri(char* uri)
{
	return print_to_buffer_html_one_level(uri);
}

static int __check_request_type(char* request)
{
	if (request == NULL || strlen(request) < 4)
		return -1;

	if (!(*(request) == 'G' && *(request+1) == 'E' && *(request+2) == 'T' && *(request+3) == ' '))
		return -1;

	return 0;
}

static int __extract_uri(char* dest, char* read_buff)
{
		char* 		uri_ptr = read_buff;
		int 		uri_len = 0;

		while (*uri_ptr != '/' && *uri_ptr != '\0')
			uri_ptr++;

		uri_ptr++;

		while (*(uri_ptr + uri_len) != ' ' && *(uri_ptr + uri_len) != '\0')
			uri_len++;

		for (int i = 0; i + 3 < uri_len; i++)
		{
			if (*(uri_ptr + i) == '/' && *(uri_ptr + i+1) == '.' && *(uri_ptr + i+2) == '.' && *(uri_ptr + i+3) == '/')
			{
				return -1;
			}
		}

		*(dest) = '.';
		*(dest + 1) = '/';

		memcpy(dest+2, uri_ptr, uri_len);

		*(dest + 2 + uri_len) = '\0';

		return 0;
}



int run_server()
{
	int                                server_fd, new_socket;
    ssize_t                            valread;
    struct sockaddr_in                 address;
    int                                opt = 1;
    socklen_t                          addrlen = sizeof(address);
    char                               buffer[BUFF_SIZE] = { 0 }, uri[BUFF_SIZE] = {0};
    char                               *out_buffer;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
        perror("socket creation failed\n");
        return -1;
    }

    // Forcefully attaching socket to the port 80
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	{
        perror("setsockopt\n");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 80
    if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) 
	{
        perror("bind failed\n");
        return -1;
    }
    if (listen(server_fd, 3) < 0)
	{
        perror("listen failed\n");
        return -1;
    }

	while (1)
	{
		if ((new_socket = accept(server_fd, (struct sockaddr*) &address, &addrlen)) < 0)
		{
        	perror("accept failed\n");
        	return -1;
    	}

    	valread = read(new_socket, buffer, BUFF_SIZE-1); // subtract 1 for the null

		if(__check_request_type(buffer) != 0)
		{
			send(new_socket, FORBIDDEN_ONLY_GET, strlen(FORBIDDEN_ONLY_GET), 0);
			printf("%b %s\n", address.sin_addr.s_addr, "attempted not GET");
			close(new_socket);
			continue;
		}

		if (__extract_uri(uri, buffer) != 0)
		{
			send(new_socket, FORBIDDEN_NO_BACKWARDS, strlen(FORBIDDEN_NO_BACKWARDS), 0);
			printf("%b %s\n", address.sin_addr.s_addr, "attempted /../ in uri");
			close(new_socket);
			continue;
		}

		printf("%s %b %s %.*s\n", "got request from", address.sin_addr.s_addr, "with uri", (int) strlen(uri), uri);

		out_buffer = __get_html_buffer_by_uri(uri);

	   	send(new_socket, out_buffer, strlen(out_buffer), 0);

		printf("%s\n", "Sent response");

		close(new_socket);

	}


    return 0;
}





#endif
