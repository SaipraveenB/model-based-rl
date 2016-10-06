/* 
 * RL Glue based Dyna agent.
 */

#include <stdio.h>  /* for printf */
#include <string.h> /* for strcmp */
#include <time.h> /*for time()*/
#include <rlglue/Agent_common.h> /* agent_ function prototypes and RL-Glue types */
#include <rlglue/utils/C/RLStruct_util.h> /* helpful functions for allocating structs and cleaning them up */

#include "models/msrbm/msrbm.h"
#include "generator/psr/psr.h"
#include "generator/generator.h"
#include "models/model.h"
#include "simulator/simulate.h"
#include "utils.h"
#include "putils.h"


#define EPSILON 0.15
#define GAMMA 0.9

action_t this_action;
action_t last_action;

float alpha = 0.5;

#define MAX_X 12
#define MAX_Y 12
#define MAX_ACTIONS 5

#define VISIBILITY_KERNEL 3
// Use it to record what we've seen. size = MAX_X * MAX_Y
int* history;

// Use it to record if we've seen anything.
int* mask;
VisibleState* vs;
VisibleState* buffer;
MSRBM* rbm;

void agent_init(const char* task_spec)
{
	/*Seed the random number generator*/
	srand(time(0));
	/*Here is where you might allocate storage for parameters (value function or policy, last action, last observation, etc)*/
	//q = new double[ MAX_X * MAX_Y * MAX_ACTIONS ];
	/*Here you would parse the task spec if you felt like it*/
	
	/*Allocate memory for a one-dimensional integer action using utility functions from RLStruct_util*/
	allocateRLStruct(&this_action,1,0,0);
	/* That is equivalent to:
			 this_action.numInts     =  1;
			 this_action.intArray    = (int*)calloc(1,sizeof(int));
			 this_action.numDoubles  = 0;
			 this_action.doubleArray = 0;
			 this_action.numChars    = 0;
			 this_action.charArray   = 0;
	*/
    rbm = new MSRBM( 40, MAX_X * MAX_Y, 2 );
    
    vs = new VisibleState( MAX_X * MAX_Y );
    vs->resetMask();

    buffer = new VisibleState( MAX_X * MAX_Y );
    buffer->resetMask();

    history = vs->values;
    mask = vs->mask;
    
    // Pretraining.
#ifdef PRETRAIN
    
    Generator* g = new PSR( MAX_X, MAX_Y, 10, 10 );
    Field* f = new Field( MAX_X, MAX_Y );
    
    VisibleState* vstemp = new VisibleState( MAX_X * MAX_Y );
    vstemp->setMask(); 
    for( int i = 0; i < PRETRAIN_SAMPLES ; i++ ){
        // Pull a full sample.
        g->generate( f );
        vstemp->values = f->items;
        
        rbm->train( vstemp );
    }
    
    delete g;
    delete f;
    delete vstemp;

#endif
}

const action_t *agent_start(const observation_t *this_observation) {
    

    dbg( 3, "Resetting History.. " );
    vs->resetMask();
    vs->clearValues();
    /* In a real action you might want to store the last observation and last action*/
	replaceRLStruct(&this_action, &last_action);
	
	return &this_action;
}

const action_t *agent_step(double reward, const observation_t *this_observation) {

    // Add observation to the relevant cells in the visualization.

    // Take a sample from the RBM for the unobserved parts of the cells.
    int cX = this_observation->intArray[ 0 ];
    int cY = this_observation->intArray[ 1 ];
    
    for( int i = -VISIBILITY_KERNEL; i < VISIBILITY_KERNEL + 1; i++ ){
        for( int j = -VISIBILITY_KERNEL; j < VISIBILITY_KERNEL + 1; j++ ){
            if( cX + i >= MAX_X || cX + i < 0 || cY + j >= MAX_Y || cY + j < 0 )
                continue;
            
            vs->values[(cX + i) * MAX_Y + cY + j] = this_observation->intArray[ 2 + ( i + VISIBILITY_KERNEL ) * (VISIBILITY_KERNEL * 2 + 1)  + ( j + VISIBILITY_KERNEL ) ];
            vs->mask[(cX + i) * MAX_Y + cY + j] = 1;
        }
    }
    
    buffer->resetMask();

    //printf("\n");
    //printVS( vs->mask, MAX_X, MAX_Y );
    
    rbm->resample( vs, buffer );
    
    //printf("IMAGINED: ");
    //printVisibleState( buffer->values, MAX_X, MAX_Y );
    //printf("\n");
    //printVisibleState( vs->mask, MAX_X, MAX_Y );
    printVSI( vs, buffer, MAX_X, MAX_Y, cX, cY );
    
    
    // Simulate Q-learning with target top right.
    int action = simulate( buffer->values, vs->mask, MAX_X, MAX_Y, cX, cY, MAX_X - 1, MAX_Y - 1 ); 
    
    //printf("rand: %lf", doubleRand()); 
    if( EPSILON > doubleRand() )
        action = randInRange( 3 ) + 1;

    // Pick the best current move for the current cell.
    this_action.intArray[0] = action;

	/* In a real action you might want to store the last observation and last action*/
	replaceRLStruct(&this_action, &last_action);

	return &this_action;
}


void agent_end(double reward) {
    
    
    // Add observation to the relevant cells.
    // printVS( vs, MAX_X, MAX_Y ); 

    //printf("**");
    printVSI( vs, buffer, MAX_X, MAX_Y, MAX_X-1, MAX_Y-1 );

    
    dbg( 3, "Training.." );
    // Record the 20x20 sample( partially observed ). split it into 'mask' and 'values'. then train the RBM with it.
    rbm->train( vs );

    dbg( 3, "Resetting History.. " );
    vs->resetMask();
    vs->clearValues();
    clearRLStruct(&last_action);

}

void agent_cleanup() {
	
    clearRLStruct(&this_action);
	clearRLStruct(&last_action);

}

const char* agent_message(const char* inMessage) {
	if(strcmp(inMessage,"what is your name?")==0)
		return "THE TERMINATOR!";
    
    if(inMessage[0] == 'a' && inMessage[1] == ' '){
        alpha = atof( inMessage + 2 );
        return "Updated Training Rate";
    }
     
	return "I don't know how to respond to your message";
}
