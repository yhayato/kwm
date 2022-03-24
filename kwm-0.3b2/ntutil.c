#include "nt.h"
#include <stdlib.h>
#include <stdio.h>

int 
nterror(s)
	char           *s;
{
	fprintf(stderr, s);
	exit(1);
}
