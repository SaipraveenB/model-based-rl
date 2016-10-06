
#include "psr.h"
#include "../../putils.h"

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
void PSR::generate(Field* field)
{
    field->clear();    

    if(doubleRand() < 0.5)
    {
        //generate inverted L
        for(int i=1;i<field->height;i++)
        {
            field->items[i*field->height + field->width-2] = 1;
            field->items[i*field->height + (field->width-3)] = 1;
        }
        for(int i=3;i<field->width-1;i++)
        {
            field->items[field->height + i] = 1;
            field->items[2*field->height + i] = 1;
        }
    }
    else
    {
        for(int i=3;i<field->height-1;i++)
        {
            field->items[i*field->height + 1] = 1;
            field->items[i*field->height + 2] = 1;
        }
        for(int i=1;i<field->width;i++)
        {
            field->items[(field->height-3)*field->height + i]=1;
            field->items[(field->height-2)*field->height + i]=1;
        }
    }
    return;


}
/*
void PSR::generate( Field* field ){
    
    field->clear();

    int* buffer = field->items;


    //comment this
    if( doubleRand() > 0.6 )
        buildHStructure( field, width/2 + 2, height/4 );
    
    
    if( doubleRand() > 0.6 )
        buildHStructure( field, width/2 - 2, height*3/4 );
    
    
    if( doubleRand() > 0.6 )
        buildVStructure( field, width/4, height/2 - 2 );

    if( doubleRand() > 0.6 )
        buildVStructure( field, width*3/4, height/2 + 2 );
    //comment this
    
    //printf("ERROR\n");
    double selector = doubleRand();

    if( selector < 0.25 )
        buildHStructure( field, width/2 + 2, height/4, hutwidth ); 
    else if( selector < 0.5 )
        buildHStructure( field, width/2 - 2, height*3/4, hutwidth );
    else if( selector < 0.75 )
        buildVStructure( field, width/4, height/2 - 2, hutwidth );
    else if( selector <= 1.00 )
        buildVStructure( field, width*3/4, height/2 + 2, hutwidth );
    
}*/
