#include "simulate.h"
#include <ctime>
#include <cstdlib>
#include <stack>
#include <tuple>
#include <algorithm>
#include <vector>

#include "../utils.h"

#define ALPHA 0.3
#define GAMMA 0.9

int get_best_action( int* env, int cx, int cy, int w, int h ){
    
    int poss[4];

    poss[0] = poss[1] = poss[2] = poss[3] = 1;
    if( env[ ( cx + 0 ) * h + cy + 1 ] )// || ( cy >= h-1 ) )
        poss[0] = 0;
    else if( env[ ( cx + 0 ) * h + cy - 1 ] )// || ( cy <= 0 ) )
        poss[1] = 0;
    else if( env[ ( cx - 1 ) * h + cy + 0 ] )//|| ( cx <= 0 ) )
        poss[2] = 0;
    else if( env[ ( cx + 1 ) * h + cy + 0 ] )//|| ( cx >= w-1 ))
        poss[3] = 0;

    //printf("%d, %d, %d, %d\n", poss[0], poss[1], poss[2], poss[3]); 
    int bypass = rand()%12 + 1;
    for( int i = 0; i < 12; i++ ){
        if( poss[ i%4 ] ) 
            bypass--;

        if( !bypass )
            return i%4 + 1;
    }
    return 0; 
}
//map : 1 - obstacle, 0 - free
//actions : 0-stay,1-up,2-down,3-left,4-right
int simulate(int *map, int* mask, const int h, const int w, const int x, const int y, const int tx, const int ty)
{
    //seed RNG
    //srand(time(NULL));

    // make 2D map for ease
    int **env = new int*[w];
    int **emask = new int*[w];

    for(int i=0;i<w;i++){
        env[i] = &(map[i*h]);
        emask[i] = &(mask[i*h]);
    }
    const float outside_penalty = -10;
    const int actions[][2] = {{0,0},{0,1},{0,-1},{-1,0},{1,0}};
   
    //value functions
    auto vfuncs = std::vector<std::vector<float*>>();
    for( int i = 0; i < h; i++ ){
        auto harr = std::vector<float*>();
        for( int j = 0;j < w;j ++ ){
            auto aarr = new float[5];
            harr.push_back( aarr );
        }
        
        vfuncs.push_back( harr );
    }

    //trajectory
    
    
    const int max_tlen = 10*h*w;
#pragma omp parallel for
    for(int i=0;i<NUM_TRAJECTORIES;i++)
    {
        std::stack<std::tuple<int,int,int,float>> traj;
        //generate a trajectory using uniformly random policy
        int tlen = 0;
        int cx = x, cy = y;
        int nx=x, ny=y;
        //printf("TRAJ %d, %d\n", cx, cy);
        float cur_reward = 0;
        for(int tlen = 0; !(cx == tx && cy == ty) ; tlen++)
        {
            //printf("BOOHOOH\n");
            // Use %5+0 for including  the 'Stay' action.
            // This action is to be used only if the system evolves with time.
            //int caction = get_best_action( map, cx, cy, w, h );
            int caction = rand()%4+1;
            
            nx += actions[caction][0];
            ny += actions[caction][1];
            cur_reward = -1;

            //bounds check
            if(nx < 0 || nx >= w)
            {
                cur_reward = outside_penalty;
                nx = (nx < 0 ? 0 : w-1);
            }
            else if(ny < 0 || ny >= h)
            {
                cur_reward = outside_penalty;
                ny = (ny < 0 ? 0 : h-1);
            }
            else if(env[nx][ny] && emask[nx][ny])
            {
                cur_reward = -10;
                nx = cx;
                ny = cy;
            
            }else if(env[nx][ny] && !emask[nx][ny]){
                cur_reward = -200;
                //nx = cx;
                //ny = cy;
            }


            
            if(tlen == max_tlen)
            {
                //simulate one more step
                int nnaction = rand()%5+0;
                int nnx = nx + actions[nnaction][0];
                int nny = ny + actions[nnaction][1];
                nnx = (nnx < 0 ? 0 : nnx >= w ? w-1:nnx);
                nny = (nny < 0 ? 0 : nny >= h ? h-1:nny);
               
                traj.push(std::make_tuple(nx,ny,caction,cur_reward+GAMMA*vfuncs[nnx][nny][nnaction]));
                break;
            }
            
            if( (nx == tx) && (ny == ty) )
                cur_reward = 1000;

            traj.push(std::make_tuple(cx,cy,caction,cur_reward));
            cx = nx;
            cy = ny;
        }
        //printf("I'm OUT!");
        int sx, sy, sa;
        float sr,pval=0.0f, target, td_error;
        
        //make TD backups to vfuncs using omp critical
        //make backup of last step
        while(traj.size())
        {
            //printf("BLALALA\n"); 
            std::tie(sx,sy,sa,sr) = traj.top();
            
            #pragma omp critical
            {
                vfuncs[sx][sy][sa] += ALPHA*(sr + GAMMA*pval - vfuncs[sx][sy][sa]);
                pval = *std::max_element( vfuncs[sx][sy], vfuncs[sx][sy] + 5 ); 
            }

            traj.pop();

        }

    }

    // find max action
    /*int max_action = 0;
    int max_value = vfuncs[x][y][0];
    for(int i=1;i<5;i++){
        if(vfuncs[x][y][i] > max_value)
        {
            max_value = vfuncs[x][y][i];
            max_action = i;
        }
    }*/
    vfuncs[x][y][0] = -100;
    if( (y >= h-1) || env[ ( x + 0 ) ][ y + 1 ] )// || ( cy >= h-1 ) )
        vfuncs[x][y][1] = -100;
    else if( (y <= 0)   || env[ ( x + 0 ) ] [ y - 1 ] )// || ( cy <= 0 ) )
        vfuncs[x][y][2] = -100;
    else if( (x <= 0 )  || env[ ( x - 1 ) ] [ y + 0 ] )//|| ( cx <= 0 ) )
        vfuncs[x][y][3] = -100;
    else if( (x >= w-1) ||env[ ( x + 1 ) ] [ y + 0 ] )//|| ( cx >= w-1 ))
        vfuncs[x][y][4] = -100;

    int max_action = std::max_element( vfuncs[x][y], vfuncs[x][y] + 5 ) - vfuncs[x][y];
    
    //poss[0] = poss[1] = poss[2] = poss[3] = 1;
    
    for( int i = 0; i < 5; i++ ){
        for( int x = 0; x < w; x++ ){
            for( int y = 0; y < h; y++ ){
                dbg(4, "%.1f ", vfuncs[x][y][i]);
            }
            dbg(4,"\n");
        }
        dbg(4,"\n\n");
    }
    //delete[] vfuncs;
    delete[] env;
    
    
    return max_action;
    
}
