#include "putils.h"
#include <stdio.h>
// Complex outer product with multiple states. the output size is HxVx(S-1)
int* outer_product( int nhidden, int nvisible, int nstates, int* hidden, int* visible ){

    auto op = new int[ nhidden * nvisible ];

    for( int h = 0; h < nhidden; h++ ){

        for( int v = 0; v < nvisible; v++ ){

            for( int k = 1; k < nstates; k++ ){

                op[h * nvisible * ( nstates - 1 ) + v * ( nstates - 1 ) + k - 1 ] = ( visible[v] == k ) ? hidden[h] : 0;

            }

        }

    }

    return op;

}



int binary_softmax( double p0, double p1 ){

    double r = ( ( (double)rand() ) / RAND_MAX );
    double disc = exp( - p1 )/( exp(-p0) + exp(-p1) );

    return ( disc > r ) ? 1 : 0;

}

void softmax( double* arr, int size, double* output ){
    double denominator = 0;
    for( int i = 0; i < size; i++ ){
        denominator += ( exp( -arr[i] ) );
    }   

    for( int k = 0; k < size; k++ ){
        output[k] = exp( -arr[k] )/denominator;
    }   
}
void softmax_adjusted( double* arr, int size, double* output ){

    double denominator = 1;
    for( int i = 0; i < size-1; i++ ){
        denominator += ( exp( -arr[i] ) );
    }   

    for( int k = 1; k < size + 1; k++ ){
        output[k] = exp( -arr[k-1] )/denominator;
    }
    output[0] = 1.0 / denominator;

}

int sample_softmax( double* arr, int size ){

    double sfmax[size];
    softmax( arr, size, sfmax );

    double cumsum[size+1];
    cumsum[0] = 0;
    for( int k = 1; k < size + 1; k++ ){
        cumsum[k] = cumsum[k-1] + sfmax[k-1];
    }

    double randVal = doubleRand();
    for( int k = 1 ; k < size + 1; k++ ){
        if( cumsum[k] > randVal )
            return k - 1;
    }
    return size - 1;

}
char* cols[6] = { "\x1B[31m", "\x1B[32m", "\x1B[37m", "\x1B[37m", "\x1B[35m", "\x1B[33m"};
void printVisibleState( int* field, int width, int height ){

    for( int i = 0; i < width; i++ ){
        printf("\n");
        for( int j = 0; j < height; j++ ){
            printf("%s%s ", cols[ field[ i * height + j ] ], "\u25A0" );
        }

    }
    printf("\n\x1B[0m");

}

void printVS( VisibleState* field, int width, int height ){

    for( int i = 0; i < width; i++ ){
        printf("\n");
        for( int j = 0; j < height; j++ ){
            if( field->mask[ i * height + j ] )
                printf("%s\u25A0", cols[ field->values[ i * height + j ] ] );
            else
                printf("%s\u25A0", cols[2] );
        }

    }
    printf("\n\x1B[0m");

}

void printVSI( VisibleState* field, VisibleState* imagined, int width, int height, int x, int y ){

    for( int i = 0; i < width; i++ ){
        printf("\n");
        for( int j = 0; j < height; j++ ){
            if( field->mask[ i * height + j ] )
                printf("%s\u25A0 ", ( (x != i || y != j) ? cols[ field->values[ i * height + j ] ] : cols[5] ) );//, (char)219 );
            else
                printf("%s\u25A0 ", cols[ imagined->values[i * height + j] + 3 ] );//, (char)219);
        }

    }
    printf("\n\x1B[0m");

}

void printVisibleCoeffs( double* coeffs, int width, int height, int hidden, int states ){

    for( int g = 0; g < hidden; g++ ){
    for( int k = 1; k < states; k++ ){
        printf("HIDDEN:%d \nSTATE: %d\n", g, k );
        for( int i = 0; i < width; i++ ){
            printf("\n");
            for( int j = 0; j < height; j++ ){
                printf("%.3lf\t", coeffs[ g * width * height * ( states - 1 ) + ( i * height + j ) * ( states - 1 ) + k - 1 ]);
            }

        }
    }
    }
    printf("\n");

}

int find_min ( double* arr, int len ){
    int min = +1000;
    int pos = 0;
    for( int i = 0; i < len; i++ ){
        
        if( arr[i] < min ) {
            min = arr[i];
            pos = i;
        }

    }
    return pos;
}

void printVisibleBias( double* coeffs, int width, int height, int states ){

    for( int k = 1; k < states; k++ ){
        printf("STATE: %d\n", k);
        for( int i = 0; i < width; i++ ){
            printf("\n");
            for( int j = 0; j < height; j++ ){
                printf("%.3lf\t", coeffs[ ( i * height + j ) * ( states - 1 ) + k - 1 ]);
            }

        }
    }
    
    printf("\n");

}
int sample_softmax_adjusted( double* arr, int size ){

    double sfmax[size];
    softmax_adjusted( arr, size, sfmax );

    double cumsum[size+1];
    cumsum[0] = 0;
    for( int k = 1; k < size + 1; k++ ){
        cumsum[k] = cumsum[k-1] + sfmax[k-1];
    }

    double randVal = doubleRand();
    for( int k = 1 ; k < size + 1; k++ ){
        if( cumsum[k] > randVal )
            return k - 1;
    }
    return size - 1;

}
