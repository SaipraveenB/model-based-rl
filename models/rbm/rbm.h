#include "model/model.h"

#include <math>


/*
 * A specialized RBM implementation that can learn from partially observed visible states.
 */
class RBM : public PModel{
    
    // Statistics.
    int num_hidden;
    int num_visible;
    
    // Energy factors.
    double *w;
    double *a;
    double *b;
    
    // Training settings
    double w_spread;
    double h_spread;
    double v_spread;

    double w_rate;
    double h_rate;
    double v_rate;
    
    
public:
    
    RBM( int hidden, int visible );
    
    // Add a training point to the RBM( modifies weights using Contrastive Divergence ).
    train( VisibleState* ob );
 
    resample( VisibleState* state, VisibleState* buffer );
    
};
