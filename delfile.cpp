#include "libefs.h"

int main(int ac, char **av)
{
	if(ac != 2)
	{
		printf("\nUsage: %s <file to delete>\n\n", av[1]);
		exit(-1);
	}

	  initFS("part.dsk", "");

	  unsigned int ndx = findFile(av[1]);

	  if(ndx != FS_FILE_NOT_FOUND)
	  {
		  delFile(av[1]);
	  }
	  else{
		  printf("FILE NOT FOUND\n");
		  fflush(stdout);
		  return ndx;

	  }

	  closeFS();


	return 0;
}
