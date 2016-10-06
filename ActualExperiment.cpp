
#include <stdio.h>  /* for printf */
#include <rlglue/RL_glue.h> /* RL_ function prototypes and RL-Glue types */
#include <iostream>
#include <fstream>
#include <sstream>
int whichEpisode = 0;

void runEpisode(int stepLimit, std::ofstream& os ) {    
    int terminal=RL_episode(stepLimit);
    printf("Episode %d\t %d steps \t%f total reward\t %d natural end \n",whichEpisode,RL_num_steps(),RL_return(), terminal);
    os<<whichEpisode<<","<<RL_num_steps()<<","<<RL_return()<<std::endl;
    whichEpisode++;
}

int main(int argc, char *argv[]) {
    const char* task_spec;
    const char* responseMessage;
    const reward_observation_action_terminal_t *stepResponse;
    const observation_action_t *startResponse;

    printf("\n\nExperiment starting up!\n");

    task_spec=RL_init();
    printf("RL_init called, the environment sent task spec: %s\n",task_spec);

    printf("\n\n----------Sending some sample messages----------\n");
    /*Talk to the agent and environment a bit...*/
    responseMessage=RL_agent_message("what is your name?");
    printf("Agent responded to \"what is your name?\" with: %s\n",responseMessage);
    responseMessage=RL_agent_message("If at first you don't succeed; call it version 1.0");
    printf("Agent responded to \"If at first you don't succeed; call it version 1.0\" with: %s\n\n",responseMessage);

    responseMessage=RL_env_message("what is your name?");
    printf("Environment responded to \"what is your name?\" with: %s\n",responseMessage);
    responseMessage=RL_env_message("If at first you don't succeed; call it version 1.0");
    printf("Environment responded to \"If at first you don't succeed; call it version 1.0\" with: %s\n",responseMessage);

    int t = time( NULL );

    srand( time( NULL ) );
    std::ofstream os;
    os.open("data.dat");
    
    for( int j = 0; j < 1; j++ ){
        RL_init();
        //std::stringstream ss;
        //ss << "a " << alphas[k] ;
        //responseMessage = RL_agent_message( ss.str().c_str() );
        //std::cout<<"RESPONSE: "<< responseMessage;
        for( int i = 0; i < 500; i++ ){
            runEpisode(110, os);
        }
        RL_cleanup();
    }

    os.close();

}
