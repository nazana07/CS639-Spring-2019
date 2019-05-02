#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void countchar (char);

int
main(int argc, char *argv[])
{
	char c;
	
	printf ("Starting.\n");

	c = 'a';
	countchar(c);
	printf ("countchar(%c)\n", c);

	c = '';
	countchar(c);
	printf ("countchar(%c)\n", c);

	c = -127;
	countchar(c);
	printf ("countchar(%c)\n", c);
	
	printf ("Done.\n");

	return 0;
}
