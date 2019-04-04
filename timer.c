/* ------------------------- 
* ESTR 3106 - Assignment 1 
* Name : Huzeyfe KIRAN
* Student ID : 1155104019 
* Email : 1155104019@link.cuhk.edu.hk
* 
* Success 
* -----------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "message.h"
#include "simpl.h"

int main(int argc, char *argv[]){
	int fd;
	MESSAGE msg, reply;
	char name[] = "Timer X";

	if(argv[1][0] == '0'){
		name[6] = '0'; //Mine

		if(name_attach(name, NULL) == -1){
			fprintf(stderr, "Cannot attach name!\n");
			exit(0);
		}

		if((fd = name_locate("Game_Admin")) == -1){
			fprintf(stderr, "Cannot locate name!\n");
			exit(0);
		}

		msg.type = REGISTER_TIMER; //register to Game_Admin
		if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
			fprintf(stderr, "Cannot register %s", name);
			exit(0);
		}

		if(!(reply.type == INIT && reply.timer_type == MINE_TIMER)){
			fprintf(stderr, "Error in receiving reply from Game_Admin!\n");
			exit(0);
		}
	}
	else if(argv[1][0] == '1'){
		name[6] = '1'; //Lancer

		if(name_attach(name, NULL) == -1){
			fprintf(stderr, "Cannot attach name!\n");
			exit(0);
		}

		if((fd = name_locate("Game_Admin")) == -1){
			fprintf(stderr, "Cannot locate name!\n");
			exit(0);
		}

		msg.type = REGISTER_TIMER; //register to Game_Admin
		if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
			fprintf(stderr, "Cannot register %s", name);
			exit(0);
		}

		if(!(reply.type == INIT && reply.timer_type == LANCER_TIMER)){
			fprintf(stderr, "Error in receiving reply from Game_Admin!\n");
			exit(0);
		}
	}
	else if(argv[1][0] == '2'){
		name[6] = '2'; //Hoplite

		if(name_attach(name, NULL) == -1){
			fprintf(stderr, "Cannot attach name!\n");
			exit(0);
		}

		if((fd = name_locate("Game_Admin")) == -1){
			fprintf(stderr, "Cannot locate name!\n");
			exit(0);
		}

		msg.type = REGISTER_TIMER; //register to Game_Admin
		if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
			fprintf(stderr, "Cannot register %s", name);
			exit(0);
		}

		if(!(reply.type == INIT && reply.timer_type == HOPLITE_TIMER)){
			fprintf(stderr, "Error in receiving reply from Game_Admin!\n");
			exit(0);
		}
	}

	TIMER_TYPE timer_type = reply.timer_type;
	
	while(reply.type != END){
		msg.type = TIMER_READY;
		msg.timer_type = timer_type;
		if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){ // tell Game_Admin that timer is ready
			fprintf(stderr, "Cannot register %s", name);
			exit(0);
		}
		if(reply.type == SLEEP && reply.timer_type == timer_type){
			useconds_t t = reply.interval * 1000;
			usleep(t);
		}
	}

	if(name_detach() == -1){
		fprintf(stderr, "Cannot detach name!\n");
		exit(0);
	}
}