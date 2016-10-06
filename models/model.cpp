#include "model.h"

VisibleState::VisibleState( int size ){
    this->values = new int[size];
    this->mask = new int[size];
    
    this->size = size;
}

void VisibleState::setMask(){
    for( int i = 0; i < size; i++ ) this->mask[i] = 1;
}

void VisibleState::resetMask(){
    for( int i = 0; i < size; i++ ) this->mask[i] = 0;
}

void VisibleState::clearValues(){
    for( int i = 0; i < size; i++ ) this->values[i] = 0;
}
