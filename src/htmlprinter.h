
int test(int a)
{
	return 2 * a;
}

long get_file_length(FILE *fptr)
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

int get_file_content(FILE* fptr, char* buffer)
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
