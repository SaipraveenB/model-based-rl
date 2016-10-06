#include "generator.h"
/*class Field{
    
    int width;
    int height;

    // Items on the field.
    int* items;
    public:
    */
Field::Field( int width, int height ){
    this->width = width;
    this->height = height;
    items = new int[ width * height ];
}

void Field::clear(){
    for( int i = 0; i < width; i++ ){
        for( int j =0; j < height; j++ ){
            items[i * height + j] = 0;
        }
    }
}

