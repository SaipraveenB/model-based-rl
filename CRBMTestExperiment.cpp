#include "models/crbm/crbm.h"
#include "models/crbm/layers/conv.h"

#include "generator/onehut/onehut.h"
#include "generator/psr/psr.h"
#include "generator/generator.h"
#include "putils.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
int main( int argc, char** argv ){
    
    OneHut* oh = new OneHut( 21, 21, 10, 10 );
   
    PSR* psr = new PSR( 21, 21, 10, 10 );
   
    Field* f = new Field( 21, 21 );
    
    srand( time( NULL ) );

    CLayer* layer = new ConvLayer( 5, 5, 5, 1, 3, 4, 1, 2 );
    CRBM* rbm = new CRBM();
    rbm->add_layer( layer );
    
    VisibleState* vs = new VisibleState( 400 );
    vs->setMask();
    
    printf("Random sampling problem domain.");
    
    for( int i = 0; i < 10000; i++ ){
        psr->generate( f );

        vs->values = f->items;

        printVisibleState( vs->values, 20, 20 );
        
        ConvState* cs = new ConvState( 20, 20, 1 );
        memcpy( cs->values, vs->values, 20 * 20 * sizeof( int ) );
        rbm->train( cs );
        
    }
    
    VisibleState* sample = new VisibleState( 400 );
    
    VisibleState* buffer = new VisibleState( 400 );
    buffer->resetMask();
    buffer->values[ 5 * 20 + 10 ] = 1;
    buffer->values[ 5 * 20 + 9 ] = 1;
    buffer->values[ 5 * 20 + 11 ] = 1;
    buffer->values[ 5 * 20 + 12 ] = 1;
    buffer->mask[ 5 * 20 + 10 ] = 1;
    buffer->mask[ 5 * 20 + 9 ] = 1;
    buffer->mask[ 5 * 20 + 11 ] = 1;
    buffer->mask[ 5 * 20 + 12 ] = 1;
    
    printf("OBSERVED: \n");
    printVisibleState( buffer->values, 20, 20 );

    for( int i =0 ; i < 2; i++ ){

        //rbm->sample( buffer, sample ); 
        printf("Sample: \n");
        printVisibleState( sample->values, 20, 20 );
    
    }

}
