#ifndef PUTILS_H
#define PUTILS_H
#include <math.h>
#include <stdlib.h>
#include "utils.h"
#include "models/model.h"
// Complex outer product with multiple states. the output size is HxVx(S-1)
int* outer_product( int nhidden, int nvisible, int nstates, int* hidden, int* visible );
int binary_softmax( double p0, double p1 );
void softmax( double* arr, int size, double* output );
void softmax_adjusted( double* arr, int size, double* output );
int sample_softmax( double* arr, int size );
int find_min( double* arr, int size );
int sample_softmax_adjusted( double* arr, int size );

void printVisibleState( int* field, int width, int height );
void printVisibleCoeffs( double* coeffs, int width, int height, int hidden, int states );
void printVisibleBias( double* coeffs, int width, int height, int states );
void printVS( VisibleState* vs, int width, int height );
void printVSI( VisibleState* field, VisibleState* imagined, int width, int height, int x, int y );

#endif
