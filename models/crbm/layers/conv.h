
#ifndef CONV_H
#define CONV_H
#include "../crbm.h"
class ConvLayer : public CLayer{

    // Network weights.
    double* w;

    // Hidden node bias.
    double* b;
    
    // Common Visible node bias.( per-state )
    double* c;
    
    int stride;
    int ksize;
    int pool_block;

    // Training constants:
    double w_rate;
    double b_rate;
    double c_rate;

    double w_spread;
    double b_spread;
    double c_spread;

    public:
        
        ConvLayer( int, int, int, int, int, int, int, int );
        void train( ConvState* );
        void sample_visible( ConvState* hidden, ConvState* visible );
        void sample_hidden( ConvState* hidden, ConvState* buffer );
        
        void sample_visible_2( ConvState* hidden, ConvState* visible );
        void sample_hidden_2( ConvState* hidden, ConvState* buffer );
        
        void updateW( int p, int w, int h, int s, double amount );
        double& W( int, int, int, int );
        void updateB( int p, double amount );
        double& B( int );
        void updateC( int s, double amount );
        double& C( int );
        
        ConvState* getHiddenState();
        
};

#endif
