#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <libmapping.h>

int main(int argc, char **argv)
{
	int i, j, ncores;
	
	assert(argc == 2);
	ncores = atoi(argv[1]);

	printf("\n");
	for (i=0; i<ncores; i++) {
		for (j=0; j<ncores; j++) {
			printf("%llu  ", libmapping_get_communication(i, j));
		}
		printf("\n");
	}
	printf("\n");

	libmapping_clear_communication();
	
	return 0;
}
