#include "libefs.h"

int main(int ac, char **av) {
	if (ac != 2) {
		printf("\nUsage: %s <file to check>\n", av[0]);
		printf("Prints: 'R' = Read only, 'W' = Read/Write\n\n");
		exit(-1);
	}

	initFS("part.dsk", "");

	unsigned int ndx = findFile(av[1]);

	if (ndx != FS_FILE_NOT_FOUND) {
		unsigned int attr =getattr(av[1]);
		int bit = (attr >> 2) & 1;

		if(bit){
			printf("R\n");
			fflush(stdout);
		}
		else{
			printf("W\n");
			fflush(stdout);

		}



	} else {
		printf("FILE NOT FOUND");
		fflush(stdout);
		return ndx;
	}
	closeFS();
	return 0;
}
