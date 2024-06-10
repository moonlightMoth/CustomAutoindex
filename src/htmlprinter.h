
int test(int a)
{
	return 2 * a;
}

char* get_file_content(FILE* fptr, long* returnSize)
{
	int i = 0;
	char* retPtr;

	if (fptr == NULL || returnSize == NULL)
	{
		perror("NULL in get_header");
		return NULL;
	}

	if (fseek(fptr, 0, SEEK_END) < 0)
	{
		perror("Got zero size get_file_content");
		return NULL;
	}
	
	*returnSize = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	retPtr = (char*) calloc(1, (*returnSize + 1) * sizeof(char));
	if (retPtr == NULL)
	{
		perror("NULL in get_header");
		return NULL;
	}
	
	char c;
	while ((c = getc(fptr))!= EOF)
		retPtr[i++] = c;

	fclose(fptr);

	retPtr[*returnSize+1] = 0;
	
	return retPtr;
}
