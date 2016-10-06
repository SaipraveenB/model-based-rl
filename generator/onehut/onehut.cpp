
#include "onehut.h"
#include "../../putils.h"

OneHut::OneHut( int width, int height, int hutwidth, int hutheight ){
    this->width = width;
    this->height = height;

    this->hutwidth = hutwidth;
    this->hutheight = hutheight;
}

void OneHut::generate( Field* field ){
    
    field->clear();

    int startX = doubleRand() * ( width - hutwidth - 1 );
    int startY = doubleRand() * ( height - hutheight - 1 );

    int* buffer = field->items;

    // Build the hut.
    for( int i = startX; i < startX + hutwidth; i++ ){
        buffer[i * height + startY] = 1;
    }
    
    for( int i = startX; i < startX + hutwidth; i++ ){
        buffer[i * height + startY + hutheight] = 1;
    }
    
    for( int i = startY; i < startY + hutheight; i++ ){
        buffer[ startX * height + i ] = 1;
    }
    
    for( int i = startY; i < startY + hutheight; i++ ){
        buffer[ ( startX + hutwidth ) * height + i] = 1;
    }

    // Build doorways.
    buffer[ startX * height + startY + hutheight/2 ]                = 0;
    buffer[ ( startX + hutwidth ) * height + startY + hutheight/2 ] = 0;
    buffer[ ( startX + hutwidth/2 ) * height + startY ]             = 0;
    buffer[ ( startX + hutwidth/2 ) * height + startY + hutheight ] = 0;
    
}
