#include "libefs.h"
#include <typeinfo>
#include <iostream>
using namespace std;

int main(int ac, char **av) {
	if (ac != 3) {
		printf("\nUsage: %s <file to check set attrbute> <attribute>\n", av[0]);
		printf("Attribute: 'R' = Read only, 'W' = Read/Write\n\n");
		exit(-1);
	}

	initFS("part.dsk", "");

	unsigned int ndx = findFile(av[1]);

	if (ndx != FS_FILE_NOT_FOUND) {
		//get the attr first
		unsigned int attr = getattr(av[1]);


		if ((strcmp((av[2]), "r") == 0) or (strcmp((av[2]), "R") == 0)) {
//			printf("1before: %d\n", attr);
//			fflush(stdout);
			attr |= 1 << 2;
			//set file as read only
//			printf("1attr: %d\n", attr);
//			fflush(stdout);
			setattr(av[1], attr);
		} else if ((strcmp(av[2], "w") == 0) or (strcmp(av[2], "W") == 0)) {

//			printf("2before: %d\n", attr);
//			fflush(stdout);
			//set file as read/write
			attr &= ~(1 << 2);
//			printf("2attr: %d\n", attr);
//			fflush(stdout);
			setattr(av[1], attr);

		}
		updateDirectory();
	} else {
		printf("FILE NOT FOUND\n");
		fflush(stdout);
		return ndx;

	}

	closeFS();

	return 0;
}
