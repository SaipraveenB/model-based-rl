#include "models/msrbm/msrbm.h"
#include "generator/onehut/onehut.h"
#include "generator/psr/psr.h"
#include "generator/generator.h"
#include "putils.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE_X 12
#define SIZE_Y 12

int main( int argc, char** argv ){
    
    OneHut* oh = new OneHut( SIZE_X, SIZE_Y, 10, 10 );
   
    PSR* psr = new PSR( SIZE_X, SIZE_Y, 10, 10 );
   
    Field* f = new Field( SIZE_X, SIZE_Y );
    
    srand( time( NULL ) );

    MSRBM* rbm = new MSRBM( 40, SIZE_X * SIZE_Y, 2 );
    
    VisibleState* vs = new VisibleState( SIZE_X * SIZE_Y );
    vs->setMask();
    
    printf("Random sampling problem domain.");
    for( int i = 0; i < 5000; i++ ){
        psr->generate( f );

        vs->values = f->items;

        printVisibleState( vs->values, SIZE_X, SIZE_Y );
        rbm->train( vs );
        
    }
    
    VisibleState* sample = new VisibleState( SIZE_X * SIZE_Y );
    
    VisibleState* buffer = new VisibleState( SIZE_X * SIZE_Y );
    buffer->resetMask();
    buffer->values[ (SIZE_X/4-1) * SIZE_Y + SIZE_Y/2 + 1 ] = 1;
    buffer->values[ (SIZE_X/4-1) * SIZE_Y + SIZE_Y/2 ] = 1;
    buffer->values[ (SIZE_X/4-1) * SIZE_Y + SIZE_Y/2 - 1] = 1;
    buffer->values[ (SIZE_X/4-1) * SIZE_Y + SIZE_Y/2 + 2] = 1;
    buffer->mask[ (SIZE_X/4-1) * SIZE_Y + SIZE_Y/2 + 1 ] = 1;
    buffer->mask[ (SIZE_X/4-1) * SIZE_Y + SIZE_Y/2 + 0 ] = 1;
    buffer->mask[ (SIZE_X/4-1) * SIZE_Y + SIZE_Y/2 - 1 ] = 1;
    buffer->mask[ (SIZE_X/4-1) * SIZE_Y + SIZE_Y/2 + 2 ] = 1;
    
    printf("OBSERVED: \n");
    printVisibleState( buffer->values, SIZE_X, SIZE_Y );

    for( int i =0 ; i < 2; i++ ){

        rbm->resample( buffer, sample ); 
        printf("Sample: \n");
        printVisibleState( sample->values, SIZE_X, SIZE_Y );
    
    }

}
