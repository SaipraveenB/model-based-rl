
#include <stdio.h>  /* for printf */
#include <string.h> /* for strcmp */
#include <time.h> /*for time()*/

#include <stdlib.h>

#include "utils.h"
double doubleRand(){
    double r, x;
	r = ((double)rand() / ((double)(RAND_MAX)+(double)(1)));
    return r;
}
int randInRange(int max){
	double r, x;
	r = ((double)rand() / ((double)(RAND_MAX)+(double)(1)));
   	x = (r * (max+1));
	return (int)x;
}
