#include "../../putils.h"
#include "crbm.h"
#include <string.h>
#include <stdio.h>


/*class CRBM : public PModel{
    
    vector<CLayer*> *layers;
    
public:
  */  
    CRBM::CRBM( ){
       layers = new vector<CLayer*>(); 
    }
    
    // Add a training point to the RBM( modifies weights using Contrastive Divergence ).
    void CRBM::train( ConvState* ob ){
        for( auto l = layers->begin(); l != layers->end(); l++ ){
            (*l)->train( ob );
            ConvState* cs = (*l)->emptyCS();
            (*l)->sample_hidden( cs, ob );
            ob = cs;
        }
    }
 
    void CRBM::sample( ConvState* state, ConvState* buffer ){
        
        for( auto l = layers->begin(); l != layers->end(); l++ ){
            ConvState* cs = (*l)->emptyCS();
            (*l)->sample_hidden( cs, state );
            state = cs;
        }

        for( int i = 0; i < layers->size() - 1; i++ ){
            CLayer* (l) = layers->at(i);
            ConvState* ta = (l)->emptyTarget();
            (l)->sample_visible( state, ta );
            state = ta;
        }
        
        layers->back()->sample_visible( state, buffer );
        
    }

    void CRBM::map( ConvState* state, ConvState* buffer ){
        
        for( auto l = layers->begin(); l != layers->end(); l++ ){
           
            ConvState* cs = (*l)->emptyCS();
            //l->sample_map( cs, ob );
            state = cs;
        
        }
    
    }
    
    void CRBM::add_layer( CLayer* layer ){
        layers->push_back( layer );
    }

    ConvState::ConvState( int width, int height, int states ){
        this->width = width;
        this->height = height;
        this->planes = planes;
        this->values = new int[ width * height * planes ];
        this->mask = new int[ width * height * planes ];

    }
        int ConvState::get( int x, int y, int z ){
            return values[ x * height * planes + y * planes + z ];
        }
        void ConvState::put( int x, int y, int z, int val ){
            values[ x * height * planes + y * planes + z ] = val;
        }
        int ConvState::get( int x, int y ){
            return values[ x * height * planes + y * planes + 0 ]; 
        }
        void ConvState::put( int x, int y, int val ){
            values[ x * height * planes + y * planes + 0 ] = val;
        }
        void ConvState::clear(){
            for( int i = 0; i < width; i++ )
                for( int j = 0; j < height; j++ )
                    for( int k = 0; k < planes; k++ )
                        put( i, j, k, 0 );
        }

        void ConvState::mask_set( int x, int y, int z ){
            mask[ x * height * planes + y * planes + z ] = 1;
        }
        void ConvState::mask_clear( int x, int y, int z ){
            mask[ x * height * planes + y * planes + z ] = 0;
        }
        
        void ConvState::mask_clear_all(){
            for( int i = 0; i < width; i++ )
                for( int j = 0; j < height; j++ )
                    for( int k = 0; k < planes; k++ )
                        mask_clear( i, j, k );
        }

        void ConvState::mask_set_all(){
            for( int i = 0; i < width; i++ )
                for( int j = 0; j < height; j++ )
                    for( int k = 0; k < planes; k++ )
                        mask_set( i, j, k );
        }
        
//};

/*class CLayer{

    int width;
    int height;
    int planes;
    
    int target_width;
    int target_height;
    int target_planes;

    public:
*/        

CLayer::CLayer( int x, int y, int z, int tx, int ty, int tz ):width(x),height(y),planes(z),target_width(tx),target_height(ty),target_planes(tz){}


        ConvState* CLayer::emptyCS(){
            return new ConvState( width, height, planes );
        }
        ConvState* CLayer::emptyTarget(){
            return new ConvState( target_width, target_height, target_planes );
        }

//}
