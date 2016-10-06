
#ifndef GEN_H
#define GEN_H


class Field{
    
    public:
    int width;
    int height;

    // Items on the field.
    int* items;
    
    Field( int, int ); 
    void clear();
};
// Abstract base class for grid world geenrators.
class Generator{
    
    public:
    int width;
    int height;
    // Random sample from distribution of worlds.
    virtual void generate( Field* buffer ) = 0;
};
#endif
