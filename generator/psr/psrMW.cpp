
#include "psr.h"
#include "../../putils.h"
#include <stdio.h>
PSR::PSR( int width, int height, int hutwidth, int hutheight ){
    this->width = width;
    this->height = height;

    this->hutwidth = hutwidth;
    this->hutheight = hutheight;

    this->hutwidth = width/2 - 2;

}

void buildHStructure( Field* field, int startX, int startY, int hutwidth ){
    for( int i = startX - (hutwidth - 1); i < startX + hutwidth; i++ )
        for( int j = startY - 1; j < startY + 1; j++ )
            field->items[ i * field->height + j ] = 1;
}

void buildVStructure( Field* field, int startX, int startY, int hutwidth ){
    
    for( int j = startY - (hutwidth - 1); j < startY + hutwidth; j++ ) 
        for( int i = startX - 1; i < startX + 1; i++ )
            field->items[ i * field->height + j ] = 1;

}

void PSR::generate( Field* field ){
    
    field->clear();

    int* buffer = field->items;


    //double selector = doubleRand();
    
    //printf("??");
    if( doubleRand() < 0.25 )
        buildHStructure( field, width/2 + 2, height/4, hutwidth );
    if( doubleRand() < 0.25 )
        buildHStructure( field, width/2 - 2, height*3/4, hutwidth );
    if( doubleRand() < 0.25 )
        buildVStructure( field, width/4, height/2 - 2, hutwidth );
    if( doubleRand() < 0.25 )
        buildVStructure( field, width*3/4, height/2 + 2, hutwidth );        

}
