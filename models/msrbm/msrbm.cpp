#include "../../putils.h"
#include "msrbm.h"
#include <string.h>
#include <stdio.h>
/*class RBM : public PModel{
    
    // Statistics.
    int hidden;
    int visible;
    
    // Actual values.
    double **weights;
    
    
public:*/
#undef DEBUG
#undef DEBUG_2
#undef DEBUG_3

// RBM initialization.
MSRBM::MSRBM( int hidden, int visible, int states ){
    
    w = new double[hidden * visible * ( states-1 )];
    a = new double[hidden];
    b = new double[visible * (states - 1)];
    
    this->num_hidden = hidden;
    this->num_visible = visible;
    this->num_states = states;
    
    this->w_spread = 0.02;
    this->a_spread = 0.02;
    this->b_spread = 0.02;

    this->b_ibias = -5.7;
    
    for( int h = 0; h < num_hidden; h++ )
        for( int v = 0; v < num_visible; v++ )
            for( int k = 0; k < num_states - 1; k++ )
                w[ h * num_visible * (num_states - 1) + v * (num_states - 1) + k] = ( w_spread * ( 2 * doubleRand() - 1 ) );

    for( int h = 0; h < num_hidden; h++ )
        //for( int k = 0; k < num_states - 1; k++ )
            a[h] =  (  a_spread * ( 2 * doubleRand() - 1 ) );

    
    for( int v = 0; v < num_visible; v++ )
        for( int k = 0; k < num_states - 1; k++ )
            b[v * (num_states-1) + k] = ( b_spread * ( 2 * doubleRand() - 1 ) ) - b_ibias;

    
    
    this->w_rate = -0.05;
    this->a_rate = 0.002;
    this->b_rate = -0.002;
}
 
void printVector( int* arr, int size ){
    for( int i = 0; i < size; i++ ){
        printf("%d\t", arr[i]);
    }
    printf("\n");
}
void printBinaryCoeffs( double* arr, int size ){
    for( int i = 0; i < size; i++ ){
        printf("%lf\t", arr[i]);
    }
    printf("\n");
}
// Add a training point to the RBM( modifies weights using Contrastive Divergence ).
void MSRBM::train( VisibleState* ob ){

    // We use Contrastive Divergence with N steps of Gibbs sampling.
    
    if( ob->size != this->num_visible ){
        // Not supported yet.
        printf("NOT SUPPORTED");
        return;
    } 
    
    int* ob_hidden = new int[num_hidden];

    // Randomly sample unobserved states.
    sample_hidden_po( ob_hidden, ob );
#ifdef DEBUG_2
    printVector( ob_hidden, num_hidden );
    printBinaryCoeffs( a, num_hidden );
#endif
    VisibleState* overlay = new VisibleState( this->num_visible );
    
    // Resample the whole visible vector.
    //sample_visible( ob_hidden, overlay );


    // Overlay it with the observed vector.
    /*for( int i = 0; i < ob->size; i++ ){
        ob->values[i] = ob->mask[i] * ob->values[i] + ( 1 - ob->mask[i] ) * overlay->values[i];
        ob->mask[i] = 1;
    }*/
#ifdef DEBUG_3
    printf("Overlaid Training Sample: \n");
    printVisibleState( ob->values, 12, 12 );
#endif
    
    // Now all of it is assumed observed( we Gibbs sampled the unobserved variables. )
    
    

    VisibleState* ideal_visible = new VisibleState( num_visible );
    
    int* ideal_hidden = new int[num_hidden];

    memcpy( ideal_hidden, ob_hidden, sizeof( int ) * num_hidden );
    
    // Perform N steps of Gibbs Sampling and pray that the Markov chain has stabilized.
    for( int i = 0; i < CD_STEPS; i++ ){
        sample_visible( ideal_hidden, ideal_visible );
        sample_hidden( ideal_hidden, ideal_visible );
    }
    
#ifdef DEBUG
    printf("IDEAL VISIBLE: \n");
    printVisibleState( ideal_visible->values, 12, 12 );
    printf("IDEAL HIDDEN: \n");
    printVector( ideal_hidden, num_hidden );
#endif

    int* ideal_outerproduct = outer_product( num_hidden, num_visible, num_states, ideal_hidden, ideal_visible->values );
    int* ob_outerproduct = outer_product( num_hidden, num_visible, num_states, ob_hidden, ob->values );
    
    
    // Update all the states.
    for( int h = 0; h < num_hidden; h++ )
        for( int v = 0; v < num_visible; v++ )
            for( int k = 0; k < num_states-1; k++ )
                w[ h * num_visible * (num_states - 1) + v * (num_states - 1) + k] += ob->mask[v] * w_rate * ( ob_outerproduct[ h * num_visible * (num_states - 1) + v * (num_states - 1) + k] - ideal_outerproduct[ h * num_visible * (num_states - 1) + v * (num_states - 1) + k] );
    
    for( int h = 0; h < num_hidden; h++ )
        a[h] += a_rate * ( ob_hidden[h] - ideal_hidden[h] );
    
    for( int v = 0; v < num_visible; v++ )
        for( int k = 0; k < num_states - 1; k++ )
            b[v * ( num_states - 1 ) + k] += b_rate * ob->mask[v] * ( ( ( ob->values[v] == ( k + 1 ) )?1:0) - ( ( ideal_visible->values[v] == ( k + 1 ) )?1:0 ) );


}


void MSRBM::sample_visible( int* hidden, VisibleState* buffer ){
    
    for( int v = 0; v < num_visible; v ++ ){
        double f[num_states];
        
        for( int i = 0; i < num_states; i++ )
            f[i] = 0; // Initialize to 0;

        for( int h = 0; h < num_hidden; h ++ ) {
            for( int k = 1; k < num_states; k++ ){ 
                f[k] += hidden[h] * w[ h * num_visible * (num_states - 1) + v * (num_states - 1) + k - 1 ];
            }
        }

        for( int k = 0; k < num_states - 1; k++ ){
            f[k + 1] += b[v * (num_states-1) + k];
        }

        buffer->values[v] = sample_softmax( f, num_states );
    
    }

}

// Find MAP estimate for visible states.
void MSRBM::map_visible( int* hidden, VisibleState* buffer ){
    
    for( int v = 0; v < num_visible; v ++ ){
        double f[num_states];
      
        for( int i = 0; i < num_states; i++ )
            f[i] = 0; // Initialize to 0;

        for( int h = 0; h < num_hidden; h ++ ) {
            for( int k = 1; k < num_states; k++ ){ 
                f[k] += hidden[h] * w[ h * num_visible * (num_states - 1) + v * (num_states - 1) + k - 1 ];
            }
        }

        for( int k = 0; k < num_states - 1; k++ ){
            f[k + 1] += b[v * (num_states-1) + k];
        }

        buffer->values[v] = find_min( f, num_states );
    
    }

}

// FULLY OBSERVED visible states ONLY.
void MSRBM::sample_hidden( int* hidden, VisibleState* visible ){
    
    
    for( int h = 0; h <  num_hidden; h ++ ){
    
        double f[2];
        f[0] = 0; // Energy of the 0 state is 0.
        
        f[1] = 0; // Initialize to 0;
        for( int v = 0; v < num_visible; v ++ ) {
            f[1] += ( visible->values[v] ?  w[ h * num_visible * (num_states - 1) + v * (num_states - 1) + (visible->values[v] - 1) ] : 0 );
        }

        f[1] += a[h];
        
        hidden[h] = binary_softmax( f[0], f[1] );
    
    }

}

// Partially observed visible states. Unobserved states are randomly sampled from their individual potentials.
void MSRBM::sample_hidden_po( int* hidden, VisibleState* visible ){
    
    
    for( int h = 0; h <  num_hidden; h ++ ){
        double f[2];
        f[0] = 0; // Energy of the 0 state is 0.
        
        f[1] = 0; // Initialize to 0;
        for( int v = 0; v < num_visible; v ++ ) {
            // Three ways to handle missing visible symbols.
            // 1. leave it's contribution as 0: unfortunately, this makes it seem like it is actually observed as 0.
            // 2. Let contribution be weighted geometric mean of the both states( weighted by their probabilities. )
            // 3. We sample a random assignment to the unobserved visible variables and after computing ob_hidden, 
            // we resample an assignment to the unobserved variables( alone ). This will essentially make the unobeserved parts a sample from the 
            // current distribution while maintaining the parts that are from the target distribution effectively giving an expected 0 update to the unobserved parts.
            
            // We pick 3.

            if( !visible->mask[v] ){
                int sample = sample_softmax_adjusted( b + v * (num_states - 1), num_states );
                f[1] += ( sample ? w[ h*(num_states-1)*num_visible + v * (num_states-1) + (sample - 1) ] : 0 );
            }
            
            f[1] += ( visible->values[v] ?  w[ h * num_visible * (num_states - 1) + v * (num_states - 1) + (visible->values[v] - 1) ] : 0 );
        }

        f[1] += a[h];
        hidden[h] = binary_softmax( f[0], f[1] );
    }

}

// Partially find Maximum A Priori estimate.
void MSRBM::map_hidden( int* hidden, VisibleState* visible ){ 
    
    for( int h = 0; h <  num_hidden; h ++ ){
        double f[2];
        f[0] = 0; // Energy of the 0 state is 0.
        
        f[1] = 0; // Initialize to 0;
        for( int v = 0; v < num_visible; v ++ ) {
            // Three ways to handle missing visible symbols.
            // 1. leave it's contribution as 0: unfortunately, this makes it seem like it is actually observed as 0.
            // 2. Let contribution be weighted geometric mean of the both states( weighted by their probabilities. )
            // 3. We sample a random assignment to the unobserved visible variables and after computing ob_hidden, 
            // we resample an assignment to the unobserved variables( alone ). This will essentially make the unobeserved parts a sample from the 
            // current distribution while maintaining the parts that are from the target distribution effectively giving an expected 0 update to the unobserved parts.
            // We pick 3.

            if( !visible->mask[v] ){
                int sample = find_min( b + v * (num_states - 1), num_states - 1 );
                
                if( b[ v * (num_states - 1 ) + sample ] > 0 ) sample = 0;
                else sample = sample + 1;

                f[1] += ( sample ? w[ h*(num_states-1)*num_visible + v * (num_states-1) + (sample) ] : 0 );
            }
            
            f[1] += ( visible->values[v] ?  w[ h * num_visible * (num_states - 1) + v * (num_states - 1) + (visible->values[v] - 1) ] : 0 );
        }

        f[1] += a[ h ] ;
        //printf("%lf ", f[1]);
        hidden[ h ] = binary_softmax( f[0], f[1] ); //( f[0] >  f[1] ) ? 1 : 0 ;
    }
    printf("\n");
}

// Resample given an incomplete visible state.( May also be empty. empty can also be indicated using NULL )
void MSRBM::resample( VisibleState* state, VisibleState* buffer ){
    
    if( state == NULL ){
        state = new VisibleState( this->num_visible );
        state->resetMask();
    }

    int* hidden = new int[num_hidden];

    // It's partially observed sampling. So we need to initialize random values for the unobserved variables.
    map_hidden( hidden, state );
#ifdef DEBUG_2
    printf("HIDDEN SAMPLE:: \n");
    printVector( hidden, num_hidden ); 
    printf("\n");
#endif
    map_visible( hidden, buffer );
    
    for( int i = 0; i < buffer->size; i++ )
        buffer->values[i] = buffer->values[i] * ( 1 - state->mask[i] ) + state->values[i] * state->mask[i];

#ifdef DEBUG_2
    printf("FINAL BUFFER: \n");
    printVisibleState( buffer->values, 12, 12 );
    printf("\n");
    printVisibleBias( b, 12, 12, 2 );
    printVisibleCoeffs( w, 12, 12, 5, 2 );
#endif
}
