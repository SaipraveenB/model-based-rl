#include "simulate.h"
#include <ctime>
#include <cstdlib>
#include <stack>
#include <tuple>
#include <algorithm>

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


    const int max_tlen = 3*h*w;

    std::stack<std::tuple<int,int,int,float>> traj;
    //generate a trajectory using uniformly random policy
    int tlen = 0;
    int cx = x, cy = y;
    int nx=x, ny=y;
    //printf("TRAJ %d, %d\n", cx, cy);
    float cur_reward = 0;
    for(int tlen = 0; !(cx == tx && cy == ty) ; tlen++)
    {
        // Use %5+0 for including the 'Stay' action.
        // This action is to be used only if the system evolves with time.
        
        int caction = rand()%5+0; 

        nx += actions[caction][0];
        ny += actions[caction][1];
        cur_reward = -2;

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
            cur_reward = -4;
            nx = cx;
            ny = cy;
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

        vfuncs[sx][sy][sa] += ALPHA*(sr + GAMMA*pval - vfuncs[sx][sy][sa]);
        pval = *std::max_element( vfuncs[sx][sy], vfuncs[sx][sy] + 5 ); 
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

int max_action = std::max_element( vfuncs[x][y], vfuncs[x][y] + 5 ) - vfuncs[x][y];

for( int i = 0; i < 5; i++ ){
    for( int x = 0; x < w; x++ ){
        for( int y = 0; y < h; y++ ){
            dbg(1, "%.1f ", vfuncs[x][y][i]);
        }
        dbg(1,"\n");
    }
    dbg(1,"\n\n");
}
delete[] vfuncs;
delete[] env;


return max_action;

}
