
ConvLayer::ConvLayer( int w, int h, int p, int stride, int ksize, int tp, int ts ):CLayer( w,h,p ) {
    target_width = w * stride + ( ksize - 1 )/2;
    target_height = h * stride + ( ksize - 1 )/2;
    target_planes = tp;
    target_states = ts;

    this->stride = stride;
    this->ksize = ksize;

}

void ConvLayer::train( ConvState* ob ){
    
}


void ConvLayer::sample_visible( ConvState* hidden, ConvState* visible ){
    
}

void ConvLayer::sample_hidden( ConvState* hidden, ConvState* visible ){
    
}
