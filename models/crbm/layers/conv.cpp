#include "conv.h"
#include "../../../putils.h"
#include <string.h>
/*class ConvLayer : public CLayer{

  int target_width;
  int target_height;
  int target_planes;
  int target_states;

  int stride;
  int ksize;

// Network weights. (PLANES)x(KSIZE)x(KSIZE)x(STATES)
double* w;

// Hidden node bias.
double* b;

// Common Visible node bias.
double c;

public:
*/       
ConvLayer::ConvLayer( int w, int h, int p, int stride, int ksize, int pool_block, int tp, int ts ) : CLayer( w, h, p, w * stride + ( ksize - 1 )/2,  h * stride + ( ksize - 1 )/2, tp) {
    target_width = w * stride + ( ksize - 1 )/2;
    target_height = h * stride + ( ksize - 1 )/2;
    target_planes = tp;
    target_states = ts;
    //CLayer::CLayer( w,h,p, target_width, target_height, target_planes );
    
    this->pool_block = pool_block;
    this->stride = stride;
    this->ksize = ksize;
    
    this->w_spread = 0.01;
    this->b_spread = 0.01;
    this->c_spread = 0.01;
    
    for( int h = 0; h < planes * ksize * ksize * target_states; h++ )
            this->w[ h ] = ( w_spread * ( 2 * doubleRand() - 1 ) );

    for( int h = 0; h < planes; h++ )
        //for( int k = 0; k < num_states - 1; k++ )
            this->b[ h ] =  (  b_spread * ( 2 * doubleRand() - 1 ) );

    
    for( int v = 0; v < target_states; v++ )
            this->c[ v ] =  ( c_spread * ( 2 * doubleRand() - 1 ) );

    
    
    this->w_rate = -0.075;
    this->b_rate = 0.004;
    this->c_rate = -0.004;
    
}

void ConvLayer::train( ConvState* ob ){
    // We use Contrastive Divergence with N steps of Gibbs sampling.


    ConvState* ob_hidden = new ConvState( width, height, planes );

    // Randomly sample unobserved states.
    sample_hidden( ob_hidden, ob );

    ConvState* overlay = new ConvState( target_width, target_height, target_planes );

    // Resample the whole visible space.
    sample_visible( ob_hidden, overlay );


    // Overlay it with the observed space.
    for( int i = 0; i < ob->width * ob->height * ob->planes; i++ ){
        ob->values[i] = ob->mask[i] * ob->values[i] + ( 1 - ob->mask[i] ) * overlay->values[i];
        ob->mask[i] = 1;
    }

    // Now all of it is assumed observed( we Gibbs sampled the unobserved variables. )


    ConvState* ideal_visible = new ConvState( target_width, target_height, target_planes );

    ConvState* ideal_hidden = new ConvState( width, height, planes );

    memcpy( ideal_hidden->values, ob_hidden->values, sizeof( int ) * width * height * planes );

    // Perform N steps of Gibbs Sampling and pray that the Markov chain has stabilized.
    for( int i = 0; i < CD_STEPS; i++ ){

        sample_visible( ideal_hidden, ideal_visible );
        sample_hidden( ideal_hidden, ideal_visible );

    }

#ifdef DEBUG
    printf("IDEAL VISIBLE: \n");
    printVisibleState( ideal_visible->values, 20, 20 );
    printf("IDEAL HIDDEN: \n");
    printVector( ideal_hidden, num_hidden );
#endif

    int offset = ( ksize - 1 )/2;
    for( int z = 0; z < planes; z++ )
        for( int x = 0; x < width; x++ )
            for( int y = 0; y < height; y++ ){
                int ideal_hs = ideal_hidden->get( x, y, z );
                int actual_hs = ob_hidden->get( x, y, z );
                updateB( z, b_rate * ( ideal_hs - actual_hs ) );

                if( !( ideal_hs * actual_hs ) ) continue;

                int centerX = x;
                int centerY = y;

                for( int i = 0; i < ksize; i++ )
                    for( int j = 0; j < ksize; j++ )
                        for( int k = 0; k < target_planes; k++ ){

                            int ideal_vs = ideal_visible->get( i + centerX, j + centerY, k );

                            int actual_vs = ob->get( i + centerX, j + centerY, k );

                            updateW( z, i, j, ideal_vs,     w_rate * ( ideal_hs ) );
                            updateW( z, i, j, actual_vs,    - w_rate * ( actual_hs ) );

                        }


            }


    for( int i = 0; i < target_width; i++ )
        for( int j = 0; j < target_height; j++ )
            for( int k = 0; k < target_planes; k++ ){
                
                int ideal_vs = ideal_visible->get( i, j, k );
                int actual_vs = ob->get( i, j, k );

                updateC( ideal_vs,  c_rate  );
                updateC( actual_vs, - c_rate );
            }
}


void ConvLayer::updateW( int p, int w, int h, int s, double amount ){
    this->w[ p * ( ksize * ksize * target_states ) + w * ( ksize * target_states ) + h * ( target_states ) + s ] += amount;
}

double& ConvLayer::W( int p, int w, int h, int s ){
    return this->w[ p * ( ksize * ksize * target_states ) + w * ( ksize * target_states ) + h * ( target_states ) + s ];
}

void ConvLayer::updateB( int p, double amount ){
    this->b[ p ] += amount;
}

double& ConvLayer::B( int p ){
    return this->b[ p ];
}

void ConvLayer::updateC( int s, double amount ){
    this->c[s] += amount;
}

double& ConvLayer::C( int s ){
    return this->c[s];
}

void ConvLayer::sample_visible_2( ConvState* hidden, ConvState* visible ){

    double* temp = new double[ target_width * target_height * target_planes * target_states ];
    for( int z = 0; z < planes; z++ )
        for( int x = 0; x < width; x++ )
            for( int y = 0; y < height; y++ ){
                int hs = hidden->get( x, y, z );
                //int actual_hs = ob_hidden->get( x, y, z );
                if( !hs ) continue;

                int centerX = x;
                int centerY = y;

                for( int i = 0; i < ksize; i++ )
                    for( int j = 0; j < ksize; j++ )
                        for( int k = 0; k < target_planes; k++ )
                            for( int s = 0; s < target_states; s++ ){

                                temp[ ( (i + centerX) * height * planes + (j + centerY) * planes  + k ) * target_states + s  ] += W( z, i, j, s );

                            }


            }

    for( int z = 0; z < target_planes; z++ )
        for( int x = 0; x < target_width; x++ )
            for( int y = 0; y < target_height; y++ ){

                for( int s = 0; s < target_states; s++ )
                    temp[ ( x * height * planes + y * planes  + z ) ] += C( s );

                visible->put( x,y,z, sample_softmax( temp + ( (x) * height * planes + (y) * planes  + z ), target_states ) );

            }



}

// Pooeld Sampling for visible layer.
void ConvLayer::sample_visible( ConvState* hidden, ConvState* visible ){
    // We'd use the sample_visible function.

    ConvState* expanded = new ConvState( hidden->width * pool_block, hidden->height * pool_block, hidden->planes );
    expanded->clear();

    for( int i = 0; i < hidden->width; i++ )
        for( int j = 0; j < hidden->height; j++ )
            for( int k = 0; k < hidden->planes; k++ ){
                int state = hidden->get( i, j, k );
                if( !state ) continue;
                state--;
                int xoff = state / pool_block;
                int yoff = state % pool_block;
                expanded->put( i * pool_block + xoff, j * pool_block + yoff, k, 1 );
            }

    // Unpooled sampling on an (N-1)x(N-1) grid.
    sample_visible_2( expanded, visible );
}

// Pooled sampling: introduce a secondary potential.
void ConvLayer::sample_hidden( ConvState* hp, ConvState* visible ){
    
    int pw = width/pool_block;
    int ph = height/pool_block;
    int np = pool_block * pool_block + 1;
    double* sampler = new double[ (planes) * (pw) * (ph) * ( 1 + pool_block * pool_block ) ];
    
    // Clear memory.
    memset( sampler, 0, planes * pw * ph * np * sizeof( double ) );

    for( int z = 0; z < planes; z++ )
        for( int x = 0; x < width; x++ )
            for( int y = 0; y < height; y++ ){

                double energy = B( z );
                
                for( int i = 0; i < ksize; i++ )
                    for( int j = 0; j < ksize; j++ )
                        for( int k = 0; k < target_planes; k++ ){
                            int vs = visible->get( i, j, k );
                            energy += ( vs ? ( W( z, i, j, vs - 1 ) ) : 0 );
                        }
                int xi = x % pool_block;
                int yi = y % pool_block;
                int index = xi * pool_block + yi;
                sampler[ z * ph * pw * np  + (x/pool_block) * ph * np + (y/pool_block) * np + index + 1 ] = energy;
            }
    
    
    for( int z = 0; z < planes; z++ )
        for( int x = 0; x < pw; x++ )
            for( int y = 0; y < ph; y++ ){
                hp->put( x, y, z, sample_softmax( sampler + z * ( ph * pw * np ) + x * ( ph * np ) + y * np , np ) );
            }
    
}

void ConvLayer::sample_hidden_2( ConvState* hidden, ConvState* visible ){
    
    for( int z = 0; z < planes; z++ )
        for( int x = 0; x < width; x++ )
            for( int y = 0; y < height; y++ ){

                double energy = B( z );
                
                for( int i = 0; i < ksize; i++ )
                    for( int j = 0; j < ksize; j++ )
                        for( int k = 0; k < target_planes; k++ ){
                            int vs = visible->get( i, j, k );
                            energy += ( vs ? ( W( z, i, j, vs - 1 ) ) : 0 );
                        }

                hidden->put( x,y,z, binary_softmax( 0, energy ) );
            }

}
