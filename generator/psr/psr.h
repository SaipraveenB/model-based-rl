#include "../generator.h"

class PSR : public Generator {
    
    int width;
    int height;
    int hutwidth;
    int hutheight;
public:
    PSR( int width, int height, int hutwidth, int hutheight );

    void generate( Field* );
};
