#include "libefs.h"

int main(int ac, char **av)
{
	if(ac != 2)
	{
		printf("\nUsage: %s <file to delete>\n\n", av[0]);
		exit(-1);
	}

	printf("%s",av[1]);
	fflush(stdout);




	return 0;
}
