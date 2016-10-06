
#ifndef CRBM_H

#define CRBM_H

#include <math.h>
#include <vector>
#include "../model.h"
using namespace std;
#define CD_STEPS 3

class ConvState{
    public:
    int* values;
    int* mask;

    int width;
    int height;
    int planes;

    ConvState( int, int, int );
    int get( int, int, int );
    void put( int, int, int, int );
    int get( int, int );
    void put( int, int, int );
    
    void clear();
    void mask_set( int x, int y, int z );
    void mask_clear( int x, int y, int z );
    void mask_clear_all();
    void mask_set_all();

};
class CLayer{
    
    protected:
    int width;
    int height;
    int planes;

    int target_width;
    int target_height;
    int target_states;
    int target_planes;

    public:
        
        CLayer( int, int, int, int, int, int );
        virtual void train( ConvState* ) = 0;
        virtual void sample_visible( ConvState* hidden, ConvState* visible ) = 0;
        virtual void sample_hidden( ConvState* hidden, ConvState* buffer ) = 0;
        ConvState* emptyCS();
        ConvState* emptyTarget();

};
/*
 * A specialized RBM implementation that can learn from partially observed visible states.
 * MS-RBM is the extension of binary RBM and it can learn several discrete states( instead of just 2 ) 
 * in the underlying grid.
 */
class CRBM : public PModel{
    
    vector<CLayer*> *layers;

public:
    
    CRBM();
    void add_layer( CLayer* );    
    // Add a training point to the RBM( modifies weights using Contrastive Divergence ).
    void train( ConvState* ob );
 
    void sample( ConvState* state, ConvState* buffer );
    void map( ConvState* state, ConvState* buffer );
};


#endif
