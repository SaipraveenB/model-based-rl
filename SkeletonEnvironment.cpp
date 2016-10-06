/* 
*/

#include <string.h> /*strcmp*/
#include <stdio.h> /*printf*/
#include <stdlib.h>
#include <time.h> /*for time()*/

#include "utils.h"
#include <rlglue/Environment_common.h>/* env_ function prototypes and RL-Glue types */
#include <rlglue/utils/C/RLStruct_util.h> /* helpful functions for allocating structs and cleaning them up */
#include "putils.h"

#include "generator/psr/psr.h"

/* This is a very simple environment with discrete observations corresponding to states labeled {0,1,...,19,20}
   The starting state is 10.

   There are 2 actions = {0,1}.  0 decrements the state, 1 increments the state.

   The problem is episodic, ending when state 0 or 20 is reached, giving reward -1 or +1, respectively.  The reward is 0 on 
   all other steps.
   */

observation_t this_observation;
reward_observation_terminal_t this_reward_observation;

// Current X,Y states.
int cX = 0;
int cY = 0;


#define TERM_X 11
#define TERM_Y 11

#define SIZE_X 12
#define SIZE_Y 12

#define VISIBILITY_KERNEL 3

// Action effects.
//int action_e[][2] = { {0,0}, {0,-1}, {0,1}, {-1,0}, {1,0} };

const int action_e[][2] = {{0,0},{0,1},{0,-1},{-1,0},{1,0}};

Generator* generator;

Field* field;

const char* env_init()
{    
    char* task_spec="VERSION RL-Glue-3.0 PROBLEMTYPE episodic DISCOUNTFACTOR 1.0 OBSERVATIONS INTS (0 20) ACTIONS INTS (0 1)  REWARDS (-1.0 1.0)  EXTRA skeleton_environment(C/C++) by Brian Tanner.";

    /* Allocate the observation variable */
    allocateRLStruct(&this_observation,2 + ( 2 * VISIBILITY_KERNEL + 1 ) * ( 2 * VISIBILITY_KERNEL + 1 ) ,0,0);
 
    this_reward_observation.observation = &this_observation; // 0 observation.
    this_reward_observation.reward = 0; // 0 reward initially. 
    this_reward_observation.terminal = 0; // Not terminal state.
    
    generator = new PSR( SIZE_X, SIZE_Y, SIZE_X/2, SIZE_Y/2 );
    field = new Field( SIZE_X, SIZE_Y );
    
    return task_spec;
}

void populate_visible_kernel( const observation_t* ob ){
    
    dbg(1, "VISIBILITY KERNEL: \n");
    int k = 0;
    for( int i = -VISIBILITY_KERNEL; i < VISIBILITY_KERNEL + 1; i ++ ){
        for( int j = -VISIBILITY_KERNEL; j < VISIBILITY_KERNEL + 1; j ++ ){
            
            if( cX + i >= SIZE_X || cX + i < 0 || cY + j >= SIZE_Y || cY + j < 0 ){
                ob->intArray[2 + k++] = 0;
                dbg(1, "- ");

                continue;
            }

            ob->intArray[2 + k++] = field->items[(cX + i) * SIZE_Y + cY + j];
            dbg(1, "%d ", ob->intArray[2 + k-1] );
        }
        dbg(1,"\n");
    }

}

const observation_t *env_start()
{
    
    cX = 0;
    cY = 0;
    this_observation.intArray[0] = cX;
    this_observation.intArray[1] = cY;

    generator->generate( field );

    populate_visible_kernel( &this_observation );

    return &this_observation;

}

const reward_observation_terminal_t *env_step(const action_t *this_action)
{

    int action = this_action->intArray[0];
    dbg(2,"\nX,Y: %d, %d\n", cX, cY);
    dbg(2,"ACTION: %d\n", action);
    int episode_over = 0;
   
    dbg( 2, "ACTUAL:\n");
    //printVisibleState( field->items, SIZE_X, SIZE_Y );
    // Do Action.
    int oldX = cX;
    int oldY = cY;
    cX += action_e[action][0];
    cY += action_e[action][1];

    // Bounds checking..
    if( cY >= SIZE_Y )
        cY = SIZE_Y-1;
    else if( cY < 0 )
        cY = 0;

    if( cX >= SIZE_X )
        cX = SIZE_X-1;
    else if( cX < 0 )
        cX = 0;
    
    if( field->items[ cX * SIZE_Y + cY ] ){
        cX = oldX;
        cY = oldY;
    }

    // Update reward_observation struct
    this_reward_observation.observation->intArray[0] = cX;
    this_reward_observation.observation->intArray[1] = cY;

    populate_visible_kernel( this_reward_observation.observation );
    
    if( (TERM_X == cX) && (TERM_Y == cY) ){
        episode_over = 1;
    }

    if( episode_over )
        this_reward_observation.reward = 10;
    else
        this_reward_observation.reward = -0.3; // Negative reward per step so that it takes the shortest path.

    this_reward_observation.terminal = episode_over;

    return &this_reward_observation;
}

void env_cleanup()
{
    dbg(3,"Cleaning up..\n");
    clearRLStruct(&this_observation);
    delete field;
}

const char* env_message(const char* inMessage) {
    if(strcmp(inMessage,"what is your name?")==0)
        return "I AM LEGEND. Now shut up and play.";

    return ":P";
}

