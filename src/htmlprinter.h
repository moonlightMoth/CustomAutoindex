#define HEDAER_FILE "header.html"
#define FOOTER_FILE "footer.html"


long __get_file_length(FILE *fptr)
{
	long size;

	if (fptr == NULL)
	{
		perror("NULL in get_file_length");
		return -1;
	}

	if (fseek(fptr, 0, SEEK_END) < 0)
	{
		perror("Got zero size get_file_length");
		return -1;
	}

	size = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);

	return size;
}

int __get_file_content(FILE* fptr, char* buffer)
{
	int i = 0;
	char* retPtr, c;

	if (fptr == NULL)
	{
		perror("NULL in get_file_content");
		return -1;
	}

	if (retPtr == NULL)
	{
		perror("NULL in get_file_content retPtr == null");
		return -1;
	}

	while ((c = getc(fptr))!= EOF)
		buffer[i++] = c;

	return 0;
}

int __write_html()
{
	return 1;
}

int print_html()
{
	int i = 0;
    long szh, szf;
    char *header_buffer, *footer_buffer;

    FILE *header_ptr = fopen(HEDAER_FILE, "r");
    FILE *footer_ptr = fopen(FOOTER_FILE, "r");
    if (header_ptr == NULL) {
        perror("Cannot open html header file");
        return 1;
    }
    if (footer_ptr == NULL) {
        perror("Cannot open html footer file");
        return 1;
    }


    szh = __get_file_length(header_ptr);
    szf = __get_file_length(footer_ptr);

    header_buffer = (char*) calloc(1, (szh) * sizeof(char));
    footer_buffer = (char*) calloc(1, (szf) * sizeof(char));

    __get_file_content(header_ptr, header_buffer);
    __get_file_content(footer_ptr, footer_buffer);

    fclose(header_ptr);
    fclose(footer_ptr);

    if (header_buffer == NULL)
    {
        perror("Cannot read html header file");
        return 1;
    }
    if (footer_buffer == NULL)
    {
        perror("Cannot read html footer file");
        return 1;
    }

    while (header_buffer[i])
        printf("%c", header_buffer[i++]);

    i = 0;
    printf("\n");
    while (footer_buffer[i])
        printf("%c", footer_buffer[i++]);

    free(header_buffer);
    free(footer_buffer);

}
