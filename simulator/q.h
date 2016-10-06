class QSimulator{
    int width;
    int height;

    int* map;
    
    int cX;
    int cY;
    
    
public:
    // Initialize with map and values x and y.( Map is 1D so that it's easier to interface with the RBM ).
    QSimulator( int*, int, int );
    // Simulate the world and learn Q values.
    void simulate();
    // Get best action for a given X,Y
    int getAction( int, int );
}
