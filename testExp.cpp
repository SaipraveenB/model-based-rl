#include "models/msrbm/msrbm.h"
#include "generator/onehut/onehut.h"
#include "generator/psr/psr.h"
#include "generator/generator.h"
#include "putils.h"

#include <stdio.h>

int main( int argc, char** argv ){
    
    OneHut* oh = new OneHut( 20, 20, 10, 10 );
   
    PSR* psr = new PSR( 20, 20, 10, 10 );
   
    Field* f = new Field( 20, 20 );
    

    MSRBM* rbm = new MSRBM( 40, 400, 2 );
    
    VisibleState* vs = new VisibleState( 400 );
    vs->setMask();
    
    printf("Random sampling problem domain.");
    for( int i = 0; i < 1000; i++ ){
        oh->generate( f );

        vs->values = f->items;

        printVisibleState( vs->values, 20, 20 );
        rbm->train( vs );
        
    }
    
    VisibleState* sample = new VisibleState( 400 );
    rbm->resample( NULL, sample );
    
    printf("Sample: \n");
    printVisibleState( sample->values, 20, 20 );

}
