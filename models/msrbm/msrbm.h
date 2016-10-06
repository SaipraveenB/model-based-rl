#include "../model.h"

#include <math.h>

#ifndef MSRBM_H
#define MSRBM_H
#define CD_STEPS 1
/*
 * A specialized RBM implementation that can learn from partially observed visible states.
 * MS-RBM is the extension of binary RBM and it can learn several discrete states( instead of just 2 ) 
 * in the underlying grid.
 */
class MSRBM : public PModel{
    
    // Statistics.
    int num_hidden;
    int num_visible;
    int num_states;
    
    // Energy factors.
    double *w;
    double *a;
    double *b;
    
    // Training settings
    double w_spread;
    double a_spread;
    double b_spread;

    double w_rate;
    double a_rate;
    double b_rate;
    
    double b_ibias;
    // Private methods.
    void sample_hidden( int* hidden, VisibleState* buffer );
    void sample_hidden_po( int* hidden, VisibleState* buffer );
    void sample_visible( int* hidden, VisibleState* state );
    void map_visible( int* hidden, VisibleState* buffer );
    

public:
    
    MSRBM( int hidden, int visible, int states );
    
    // Add a training point to the RBM( modifies weights using Contrastive Divergence ).
    void train( VisibleState* ob );
 
    void resample( VisibleState* state, VisibleState* buffer );
    
    void map_hidden( int* hidden, VisibleState* buffer );
    
};

#endif
