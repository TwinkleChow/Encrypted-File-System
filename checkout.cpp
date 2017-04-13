#include "libefs.h"

int main(int ac, char **av)
{
	if(ac != 3)
	{
		printf("\nUsage: %s <file to check out> <password>\n\n", av[0]);
		exit(-1);
	}

	// Mount the file system
	initFS("part.dsk", av[2]);

	// Get the attributes
	TFileSystemStruct *fs = getFSInfo();

	int fp = openFile(av[1],MODE_NORMAL);

	if(fp == -1)
	{
		printf("Cannot find encrypted file %s\n", av[1]);
		exit(-1);
	}

	// Get file length
	unsigned int len = getFileLength(av[1]);

	char *buffer = makeDataBuffer();
	unsigned long *inode = makeInodeBuffer();


	readFile(fp,buffer,sizeof(char),fs->blockSize);

//	printf("buffer: %s",buffer);
//	fflush(stdout);
	closeFile(fp);





	// Open output file
	FILE *fp2 = fopen(av[1], "w");



	// Write the data
	fwrite(buffer, sizeof(char), len, fp2);

	// Close the file
	fclose(fp2);

	// Unmount the file system
	closeFS();

	free(inode);
	free(buffer);

	return 0;
}
