#include "libefs.h"

int main(int ac, char **av)
{

	if(ac != 3)
	{
		printf("\nUsage: %s <filename> <password>\n\n", av[0]);
		exit(-1);
	}

	FILE *fp = fopen(av[1], "r");


	if(fp == NULL)
	{
		printf("\nUnable to open source file %s\n\n", av[1]);
		exit(-1);
	}



	// Load the file system
	initFS("part.dsk",av[2]);

	// Get the FS metadata
	TFileSystemStruct *fs = getFSInfo();

	char *buffer;

	// Allocate the buffer for reading
	buffer = makeDataBuffer();

	// Read the file
	unsigned long len = fread(buffer, sizeof(char), fs->blockSize, fp);

	//write file


	int fp2 = openFile(av[1],MODE_NORMAL);



	if(fp2==-1){
		//file does not exit
		//create the file

		fp2 = openFile(av[1],MODE_CREATE);

		if(fp2==-1){
			//not enough space in disk
			printf("Not enough space in disk");
			exit(-1);

		}
		else{
			//file created
			//write to the newly created file
			writeFile(fp2,buffer,sizeof(char),len);
			//close the file
			closeFile(fp2);
			closeFS();
			free(buffer);
			return 0;
		}

	}
	else{
		//file already exist
		printf("DUPLICATE FILE ERROR");
		exit(-1);

	}


}
