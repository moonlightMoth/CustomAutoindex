#ifndef SERVER_H_SEEN
#define SERVER_H_SEEN

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>

#include "html_printer.h"

#define PORT 8080
#define BUFF_SIZE 4096
#define FORBIDDEN_NO_BACKWARDS "HTTP/1.1 403 Forbidden\nContent-Type: application/json\n\n{\n  \"error\": \"Wrong request\",\n  \"message\": \"/../ not allowed in request\"\n}\n"
#define FORBIDDEN_ONLY_GET "HTTP/1.1 403 Forbidden\nContent-Type: application/json\n\n{\n  \"error\": \"Wrong request\",\n  \"message\": \"only GET allowed\"\n}\n"
#define NOT_FOUND "HTTP/1.1 404 Not Found\n"
#define OK_HTML_HEADERS "HTTP/1.1 200 OK\nServer: custom_autoindex\nContent-Type: text/html\n\n"
#define OK_FILE_HEADERS "HTTP/1.1 200 OK\nServer: custom_autoindex\nContent-Type: application/octet-stream\n"
#define FILE_LEN_HEADER "Content-Length: "

#define OK_HTML_HEADERS_LEN 66
#define OK_FILE_HEADERS_LEN 81
#define FILE_LEN_HEADER_LEN 16
#define FILE_SEND_BUFF_LEN 1048576 // len of buffer to send file
#define PTHREAD_STACK_SIZE 2097152

#ifndef DIR_IDENTITY
	#define DIR_IDENTITY 0
#endif
#ifndef FILE_IDENTITY
	#define FILE_IDENTITY 1
#endif
#ifndef LINK_IDENTITY
	#define LINK_IDENTITY 2
#endif
#define NOT_FOUND_IDENTITY -1

char *dest_wd, *exec_wd;

static int __print_time(int num_of_new_lines)
{
	time_t                    rawtime;
    char                      buffer[80];

    time(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y.%m.%d %H:%M:%S", localtime(&rawtime));

    printf("[%s] ", buffer);

	for (int i = 0; i < num_of_new_lines; i++)
	{
		printf("\n");
	}

    return 0;
}

static char* __get_html_buffer_by_uri(char* uri)
{
	char* ret = print_to_buffer_html_one_level(uri);
	if (ret == NULL)
	{
		printf("cannot make html from %s\n", uri);
		return NULL;
	}
	return ret;
}

static int __check_request_type(char* request)
{
	if (request == NULL || strlen(request) < 4)
		return -1;

	if (!(*(request) == 'G' && *(request+1) == 'E' && *(request+2) == 'T' && *(request+3) == ' '))
		return -1;

	return 0;
}


//convents 3 chars in format %AA to one char. example: %AD -> 10 * 16 + 13 = 170
static char __triplet_hex_to_char(char* triplet)
{
	char ret;

	if (48 <= triplet[1] && triplet[1] <= 57)
	{
		ret = triplet[1] - 48;
		ret *= 16;
	}
	else if (65 <= triplet[1] && triplet[1] <= 90)
	{
		ret = triplet[1] - 65 + 10;
		ret *= 16;
	}
	else
	{
		printf("cannot convert %*.s to decimal\n", 3, triplet);
		return 255;
	}


	if (48 <= triplet[2] && triplet[2] <= 57)
	{
		ret += triplet[2] - 48;
	}
	else if (65 <= triplet[2] && triplet[2] <= 90)
	{
		ret += triplet[2] - 65 + 10;
	}
	else
	{
		printf("cannot convert %*.s to decimal\n", 3, triplet);
		return 255;
	}

	return ret;

}

static int __parse_uri(char* uri)
{
	int             len, i, insert_ptr = 0;


	len = strlen(uri);

	for (i = 0; i < len; i++)
	{
		if(uri[i] == '%')
		{
			uri[insert_ptr] = __triplet_hex_to_char((uri+i));

			if (uri[insert_ptr] == 255)
			{
				return -1;
			}

			insert_ptr++;
			i+=2;
		}
		else
		{
			uri[insert_ptr] = uri[i];
			insert_ptr++;
		}
	}

	uri[insert_ptr] = '\0';
//	printf("%s\n", uri);

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


		if (uri_len == 0)
		{
			dest[0] = '.';
			uri_len++;
		}
		else
		{
			memcpy(dest, uri_ptr, uri_len);
		}

		dest[uri_len] = '\0';

		if(__parse_uri(dest) != 0)
		{
			return -2;
		}

		return 0;
}

static int __send_ok_dir_by_uri(char* uri, int out_socket)
{
	char*                              out_buffer;

	if((out_buffer = __get_html_buffer_by_uri(uri)) == NULL)
	{
		send(out_socket, NOT_FOUND, strlen(NOT_FOUND), 0);
		return -1;
	}

	send(out_socket, OK_HTML_HEADERS, OK_HTML_HEADERS_LEN, 0);
	send(out_socket, out_buffer, strlen(out_buffer),0);

	free(out_buffer);

	return 0;

}

static int __send_ok_file_by_uri(char* uri, int out_socket)
{
	FILE                              *file;
	char                              buffer[FILE_SEND_BUFF_LEN] = {0}, sz_buff[100] = {0};
	unsigned long                     last_count, sz;

	chdir(dest_wd);

	if((file = fopen(uri, "r")) == NULL)
	{
		send(out_socket, NOT_FOUND, strlen(NOT_FOUND), 0);
		return -1;
	}

	fseek(file, 0L, SEEK_END);
	sz = ftell(file);
	fseek(file, 0L, SEEK_SET);
	rewind(file);

	sprintf(sz_buff, "%ld\n\n", sz);

	send(out_socket, OK_FILE_HEADERS, strlen(OK_FILE_HEADERS), 0);
	send(out_socket, FILE_LEN_HEADER, strlen(FILE_LEN_HEADER), 0);
	send(out_socket, sz_buff, strlen(sz_buff), 0);

	while((last_count = fread(buffer, 1, FILE_SEND_BUFF_LEN, file)) == FILE_SEND_BUFF_LEN)
	{
		send(out_socket, buffer, FILE_SEND_BUFF_LEN, 0);
	}

	send(out_socket, buffer, last_count, 0);

	fclose(file);

	return 0;
}

//check weather uri is file, dir, link or not found
static int __get_identity(char* uri)
{
	struct stat                   stat_buff;
	int                           lstatret;

	chdir(dest_wd);

	if (lstat(uri, &stat_buff) != 0)
	{
		return NOT_FOUND_IDENTITY;
	}

	if (S_ISDIR(stat_buff.st_mode))
	{
		return DIR_IDENTITY;
	}

	if(S_ISREG(stat_buff.st_mode))
	{
		return FILE_IDENTITY;
	}

	if (S_ISLNK(stat_buff.st_mode))
	{
		return LINK_IDENTITY;
	}

	return LINK_IDENTITY;
}


void* __process_request(void* socket_arg)
{

	int                                identity, extraction_code, new_socket;
    ssize_t                            valread;
    struct sockaddr_in                 address;
    socklen_t                          addrlen = sizeof(address);
    char                               buffer[BUFF_SIZE] = { 0 }, uri[BUFF_SIZE] = {0};
    char                               *out_buffer;

	new_socket = *(int*)socket_arg;

	valread = read(new_socket, buffer, BUFF_SIZE-1); // subtract 1 for the null

	if(__check_request_type(buffer) != 0)
	{
		send(new_socket, FORBIDDEN_ONLY_GET, strlen(FORBIDDEN_ONLY_GET), 0);
		__print_time(0);
		printf("%s attempted not GET\n", inet_ntoa(address.sin_addr));
		close(new_socket);
		return NULL;
	}

	if ((extraction_code = __extract_uri(uri, buffer)) == -1)
	{
		send(new_socket, FORBIDDEN_NO_BACKWARDS, strlen(FORBIDDEN_NO_BACKWARDS), 0);
		__print_time(0);
		printf("%s attempted /../ in uri\n", inet_ntoa(address.sin_addr));
		close(new_socket);
		return NULL;
	}
	else if (extraction_code == -2)
	{
		send(new_socket, NOT_FOUND, strlen(NOT_FOUND), 0);
		__print_time(0);
		printf("%s cannot parse uri\n", inet_ntoa(address.sin_addr));
		close(new_socket);
		return NULL;
	}

	__print_time(0);
	printf("got request from %s with uri %s\n", inet_ntoa(address.sin_addr), uri);

	identity = __get_identity(uri);

	switch (identity)
	{
		case DIR_IDENTITY: __send_ok_dir_by_uri(uri, new_socket); break;
		case FILE_IDENTITY: __send_ok_file_by_uri(uri, new_socket); break;
		case LINK_IDENTITY: __send_ok_dir_by_uri(uri, new_socket); break;
		default: send(new_socket, NOT_FOUND, strlen(NOT_FOUND), 0);
				 __print_time(0);
				 printf("%s attempted access non existent dir %s\n", inet_ntoa(address.sin_addr), uri);
				 break;
	}

	close(new_socket);

	__print_time(0);
	printf("Sent response to %s\n", inet_ntoa(address.sin_addr));

	return NULL;
}


int run_server(char* dwd, char* ewd)
{
	int                                server_fd, new_socket;
    struct sockaddr_in                 address;
    socklen_t                          addrlen = sizeof(address);
    int                                opt = 1;
	pthread_t                          last_thread;
	pthread_attr_t                     attr;

	printf("running server...\n\n");

	chdir(dest_wd);

	printf("current destination directory is %s\n", dest_wd);
	printf("current executeble directory is %s\n\n", exec_wd);

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

	//create attr to make thread stack size 2 MiB
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, PTHREAD_STACK_SIZE);

	while (1)
	{
		if ((new_socket = accept(server_fd, (struct sockaddr*) &address, &addrlen)) < 0)
		{
        	perror("accept failed\n");
        	return -1;
    	}

		pthread_create(&last_thread, &attr, &__process_request, &new_socket);
		//__process_request(&new_socket);
	}
}





#endif
