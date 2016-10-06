#include "simulate.h"
#include <ctime>
#include <cstdlib>
#include <stack>
#include <tuple>
#include "../utils.h"

#define ALPHA 0.3
#define GAMMA 0.9

//map : 1 - obstacle, 0 - free
//actions : 0-stay,1-up,2-down,3-left,4-right
int simulate(int *map, int h, int w, int x, int y, int tx, int ty)
{
    //seed RNG
    //srand(time(NULL));

    // make 2D map for ease
    int **env = new int*[w];
    for(int i=0;i<w;i++)
        env[i] = &(map[i*h]);
    
    const float outside_penalty = -5;
    const int actions[][2] = {{0,0},{0,1},{0,-1},{-1,0},{1,0}};
   
    //value functions
    auto vfuncs = new float[h][w][5]();
        
    //trajectory

    
    const int max_tlen = 5*h*w;

    for(int i=0;i<NUM_TRAJECTORIES;i++)
    {
        std::stack<std::tuple<int,int,int,float>> traj;
        //generate a trajectory using uniformly random policy
        int tlen = 0;
        int cx = x, cy = y;
        int nx=x, ny=y;
        float cur_reward = 0;
        for(int tlen = 0;(cx == tx && cy == ty); tlen++)
        {
            // Use %5+0 for including the 'Stay' action.
            // This action is to be used only if the system evolves with time.
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
            else if(env[nx][ny])
            {
                cur_reward = -13;
                //nx = cx;
                //ny = cy;
            }
            
            if(tlen == max_tlen)
            {
                //simulate one more step
                int nnaction = rand()%5;
                int nnx = nx + actions[nnaction][0];
                int nny = ny + actions[nnaction][1];
                nnx = (nnx < 0 ? 0 : nnx >= w ? w-1:nnx);
                nny = (nny < 0 ? 0 : nny >= h ? h-1:nny);
               
                traj.push(std::make_tuple(nx,ny,caction,cur_reward+GAMMA*vfuncs[nnx][nny][nnaction]));
                break;
            }
            
            if( (nx == cx) && (ny == cy) )
                cur_reward = 100;

            traj.push(std::make_tuple(nx,ny,caction,cur_reward));
            cx = nx;
            cy = ny;
        }
        int sx, sy, sa;
        float sr,pval=0.0f, target, td_error;
        
        //make TD backups to vfuncs using omp critical
        //make backup of last step
        while(traj.size())
        {
            std::tie(sx,sy,sa,sr) = traj.top();
            vfuncs[sx][sy][sa] += ALPHA*(sr + GAMMA*pval - vfuncs[sx][sy][sa]);
            pval = vfuncs[sx][sy][sa];
            traj.pop();
        }
    }

    // find max action
    int max_action = 0;
    int max_value = vfuncs[x][y][0];
    for(int i=1;i<5;i++)
        if(vfuncs[x][y][i] > max_value)
        {
            max_value = vfuncs[x][y][i];
            max_action = i;
        }
    

    delete[] vfuncs;
    delete[] env;

    return max_action;
    
}
