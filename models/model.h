
/*
 * PModel :- Probabilistic model. Abstract base class or RBM, CRBM and any other generative model.
 * 
 */
#ifndef VS_H
#define VS_H
class VisibleState{

    public:
    // number of states.
    int size;

    // Weight of the configuration( relevant for resampling )
    double weight;

    // Mask over which states are specified and which are not obseverved/sampled.
    int* mask;

    // State values of each visible variable.
    int* values;
    
    VisibleState( int );
    void setMask();
    void resetMask();
    void clearValues();
};

class PModel{
   
    int visible; 
    
public:
    
    // Add a training point to the RBM( modifies weights using Contrastive Divergence ).
    void train( VisibleState* ob );
    
    // Resample the remaining visible states given buffer states.
    void resample( VisibleState* state, VisibleState* buffer );

};

#endif
