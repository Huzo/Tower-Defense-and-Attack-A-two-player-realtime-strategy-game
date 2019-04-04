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

	if(argv[1][0] == '0' || argv[1][0] == '1'){ // Courier 0 or 1
		int fd_i, fd_g, human_id;
		MESSAGE msg, reply;
		char name[] = "Courier X";

		if(argv[1][0] == '0'){ // Courier 0
			name[8] = '0'; // Input_Admin <=> Game_Admin
			human_id = 0;
		}
		else{ // Courier 1
			name[8] = '1'; // Input_Admin <=> Game_Admin
			human_id = 1;
		}
		
		ARENA arena; 
		if(name_attach(name, NULL) == -1){
			fprintf(stderr, "Cannot attach name!\n");
			exit(0);
		}

		if((fd_i = name_locate("Input_Admin")) == -1){
			fprintf(stderr, "Cannot locate name!\n");
			exit(0);
		}

		if((fd_g = name_locate("Game_Admin")) == -1){
			fprintf(stderr, "Cannot locate name!\n");
			exit(0);
		}

		msg.type = REGISTER_HUMAN; //register to Game Admin
		if(Send(fd_g, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
			fprintf(stderr, "Cannot register %s", name);
			exit(0);
		}
		while(reply.type != INIT || reply.humanId != human_id){
			//wait
			fprintf(stderr, "waiting.\n");
		}
		if(!(reply.type == INIT && reply.humanId == human_id)){
			fprintf(stderr, "Error in receiving reply from Game_Admin!\n");
			exit(0);
		}

		msg.type = HUMAN_READY; // notify that this is ready to Game_Admin
		msg.humanId = human_id;
		if(Send(fd_g, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
			fprintf(stderr, "Cannot notify readiness of %s", name);
			exit(0);
		}
		while(reply.type != START){
			//wait
			fprintf(stderr, "waiting\n");
		}
		if(!(reply.type == START)){
			fprintf(stderr, "Error in receiving reply from Game_Admin!\n");
			exit(0);
		}
		arena = reply.arena;

		msg.type = REGISTER_COURIER; //register to Input Admin
		if(Send(fd_i, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
			fprintf(stderr, "Cannot register %s", name);
			exit(0);
		}
		if(!(reply.type == INIT)){
			fprintf(stderr, "Error in receiving reply from Input_Admin!\n");
			exit(0);
		}
		
		human_id = reply.humanId;

		msg.type = COURIER_READY; // signal that courier is ready to Input Admin
		msg.humanId = human_id;
		if(Send(fd_i, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
			fprintf(stderr, "Cannot register %s", name);
			exit(0);
		}
		
		if(!(reply.type == REGISTER_HUMAN)){
			msg.type = FAIL;
			msg.humanId = human_id;
			Send(fd_i, &msg, &reply, sizeof(msg), sizeof(reply));
			fprintf(stderr, "Error in registering Courier!\n");
			exit(0);
		}

		msg.type = INIT; // Registration successful
		msg.humanId = human_id;
		if(Send(fd_i, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
			fprintf(stderr, "Cannot register %s", name);
			exit(0);
		}

		if(reply.type == HUMAN_READY){
			msg.type = START;
			msg.humanId = human_id;
			msg.arena = arena;
			if(Send(fd_i, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
			fprintf(stderr, "Error in Starting!\n");
			exit(0);
			}
		}

		int isGameOver = 0;
		while(!isGameOver){
			if(reply.type == HUMAN_MOVE){
				msg.type = HUMAN_MOVE;
				msg.humanId = reply.humanId;
				msg.act = reply.act;

				if(Send(fd_g, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
					fprintf(stderr, "Error in notifying Game_Admin the actions!\n");
					exit(0);
				}
				if(reply.type == UPDATE){
					msg.type = UPDATE;
					msg.humanId = reply.humanId;
					msg.arena = reply.arena;

					if(Send(fd_i, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
						fprintf(stderr, "Error in notifying Input_Admin the update!\n");
						exit(0);
					}
				}
				else if(reply.type == END){
					isGameOver = 1;

					msg.type = END;
					msg.arena = reply.arena;
					msg.humanId = human_id;
					if(Send(fd_i, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
						// input_admin already received the msg from other cour
					}
				}
			}
		}

		if(name_detach() == -1){
			fprintf(stderr, "Cannot detach name!\n");
			exit(0);
		}
	}

	else if(argv[1][0] == '2'){ // Courier 2
		int fd_d, fd_g;
		MESSAGE msg, reply;
		char name[] = "Courier 2"; // Input_Admin <=> Game_Admin

		ARENA arena;

		if(name_attach(name, NULL) == -1){
			fprintf(stderr, "Cannot attach name!\n");
			exit(0);
		}

		if((fd_d = name_locate("Display_Admin")) == -1){
			fprintf(stderr, "Cannot locate name!\n");
			exit(0);
		}

		if((fd_g = name_locate("Game_Admin")) == -1){
			fprintf(stderr, "Cannot locate name!\n");
			exit(0);
		}

		msg.type = REGISTER_COURIER; //register to Game Admin
		if(Send(fd_g, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
			fprintf(stderr, "Cannot register %s", name);
			exit(0);
		}
		if(!(reply.type == INIT)){
			fprintf(stderr, "Error in receiving reply from Input_Admin!\n");
			exit(0);
		}

		msg.type = COURIER_READY; // signal courier 2 is ready to Game_Admin
		if(Send(fd_g, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
			fprintf(stderr, "Cannot signal readiness to Game_Admin!\n");
			exit(0);
		}
		if(!(reply.type == DISPLAY_ARENA)){
			fprintf(stderr, "Error in receiving display_arena reply from Input_Admin!\n");
			exit(0);
		}
		arena = reply.arena;

		int isGameOver = 0;
		while(!isGameOver){
			msg.type = OKAY; // signal courier 2 is ready to Game_Admin
			if(Send(fd_g, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
				fprintf(stderr, "Cannot signal readiness to Game_Admin!\n");
				exit(0);
			}

			if(reply.type == DISPLAY_ARENA){ // extra caution
				arena = reply.arena;

				msg.type = DISPLAY_ARENA; // deliver arena to Display_Admin 
				msg.arena = arena;
				if(Send(fd_d, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
					fprintf(stderr, "Cannot send message to Display_Admin!\n");
					exit(0);
				}
				if(!(reply.type == OKAY)){
					fprintf(stderr, "Error in receiving approval from Display_Admin\n");
					exit(0);
				}
			}
			else if(reply.type == END){
				isGameOver = 1;

				msg.type = END;
				msg.arena = reply.arena;
				msg.humanId = reply.humanId;
				if(Send(fd_d, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
					fprintf(stderr, "Cannot signal the end to Display_Admin!\n");
					exit(0);
				}
			}
		}
		
		if(name_detach() == -1){
			fprintf(stderr, "Cannot detach name!\n");
			exit(0);
		}
	}
}