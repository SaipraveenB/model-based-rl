#include "../generator.h"

class OneHut : public Generator {
    
    int width;
    int height;
    int hutwidth;
    int hutheight;
public:
    OneHut( int width, int height, int hutwidth, int hutheight );

    void generate( Field* );
};
