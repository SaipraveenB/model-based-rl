#include "putils.h"

/*class RBM : public PModel{
    
    // Statistics.
    int hidden;
    int visible;
    
    // Actual values.
    double **weights;
    
    
public:*/

// RBM initialization.
RBM::RBM( int hidden, int visible ){
    
    w = new int[hidden][visible];
    a = new int[hidden];
    b = new int[visible];
    
    this->num_hidden = hidden;
    this->num_isible = visible;
    
    for( int h = 0; h < num_hidden; h++ )
        for( int v = 0; v < num_visible; v++ )
            w[h][v] = w_spread * double_rand();

    for( int h = 0; h < num_hidden; h++ )
        a[h] = h_spread * double_rand();
    
    for( int v = 0; v < num_visible; v++ )
        b[v] = v_spread * double_rand();
    

}
    
// Add a training point to the RBM( modifies weights using Contrastive Divergence ).
void RBM::train( VisibleState* ob ){

    // We use Contrastive Divergence with N steps o Gibbs sampling.
    
    if( ob->size != this->num_visible ){
        // Not supported yet.
        return;
    } 
    
    int* ob_hidden = new int[num_hidden];

    sample_hidden( ob_hidden, ob );
    
    int* ideal_visible = new VisibleState( num_visible );
    
    int* ideal_hidden = new int[num_hidden];
    memcpy( ideal_hidden, ob_hidden, sizeof( int ) * num_hidden );
    
    // Perform N steps of Gibbs Sampling and pray that the Markov chain has stabilized.
    for( int i = 0; i < CD_STEPS; i++ ){
        sample_visible( ideal_hidden, ideal_visible );
        sample_hidden( ideal_hidden, ideal_visible );
    }
    
    ideal_outerproduct = outer_product( num_hidden, num_visible, ideal_hidden, ideal_visible->values );
    ob_outerproduct = outer_product( num_hidden, num_visible, ob_hidden, ob->values );
    
    /*
     * Expand this:
     */
    for( int h = 0; h < num_hidden; h++ )
        for( int v = 0; v < num_visible; v++ )
            w[h][v] += w_rate * ( ob_outerproduct[h][v] - ideal_outerproduct[h][v] );
    
    for( int h = 0; h < num_hidden; h++ )
        a[h] += a_rate * ( ideal_hidden[h] );
    
    for( int v = 0; v < num_visible; v++ )
        b[v] += b_rate * ( ob_visible[v] );

}

void RBM::sample_visible( int* hidden, VisibleState* buffer ){
    
    for( int v = 0; v < num_visible; v ++ ){
        double f[2];
        f[0] = 0; // Energy of the 0 state is 0.
        
        f[1] = 0; // Initialize to 0;
        for( int h = 0; h < num_hidden; h ++ ) {
            // Two ways to handle missing visible symbols.
            // 1. leave it's contribution as 0: unfortunately, this makes it seem like it is actually observed as 0.
            // 2. Let contribution be weighted geometric mean of the both states( weighted by their probabilities. )
            // We pick 2 as it makes sense to allow an unobserved state to affect the hidden sample prediction.
 
            f[1] += hidden[h] * w[h][v];
        }
        f[1] += b[v];
        buffer->values[v] = binary_softmax( f[0], f[1] );
    }

}

void RBM::sample_hidden( int* hidden, VisibleState* visible ){
    
    // Set probability that it's 0 to a[0];
    
    for( int h = 0; h <  num_hidden; h ++ ){
        double f[2];
        f[0] = 0; // Energy of the 0 state is 0.
        
        f[1] = 0; // Initialize to 0;
        for( int v = 0; v < num_visible; v ++ ) {
            // Two ways to handle missing visible symbols.
            // 1. leave it's contribution as 0: unfortunately, this makes it seem like it is actually observed as 0.
            // 2. Let contribution be weighted geometric mean of the both states( weighted by their probabilities. )
            // 3. We sample a random assignment to the unobserved visible variables and after computing ob_hidden, 
            // we resample an assignment to the unobserved variables( alone ). This will essentially make the unobeserved parts a sample from the 
            // current distribution while maintaining the parts that are from the target distribution effectively giving an expected 0 update to the unobserved parts.
            
            // We pick 2 for now.

            if( !visible->mask[k] ){
                f[1] += ( exp( b[v] ) / ( 1 + exp( b[v] ) ) ) * w[h][v]; //Arithmetic weighted mean in the energy space.
            }
            
            f[1] += visible->values[v] * w[h][v];
        }
        f[1] += a[h];
        hidden[h] = binary_softmax( f[0], f[1] );
    }

}

// Resample given an incomplete visible state.( May also be empty. empty can also be indicated using NULL )
void RBM::resample( VisibleState* state, VisibleState* buffer ){
    
    int* hidden = new int[num_hidden];
    sample_hidden( hidden, state );
    

}
