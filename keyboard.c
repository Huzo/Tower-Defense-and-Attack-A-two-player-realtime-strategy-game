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
#include <curses.h>
#include "message.h"
#include "simpl.h"

int main(){
	int fd;
	MESSAGE msg, reply;
	char name[] = "Keyboard";

	if(name_attach(name, NULL) == -1){
		fprintf(stderr, "Cannot attach name!\n");
		exit(0);
	}

	if((fd = name_locate("Input_Admin")) == -1){
		fprintf(stderr, "Cannot locate name!\n");
		exit(0);
	}

	msg.type = REGISTER_KEYBOARD; //register to Input Admin
	if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
		fprintf(stderr, "Cannot register %s", name);
		exit(0);
	}
	if(!(reply.type == INIT)){
		fprintf(stderr, "Error in receiving reply from Input_Admin!\n");
		exit(0);
	}

	//notify that keyboard is ready.
	msg.type = KEYBOARD_READY; // notify Input Admin
	if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
		fprintf(stderr, "Cannot notify Input_Admin that Keyboard is ready!\n");
		exit(0);
	}
	if(!(reply.type == START)){
		fprintf(stderr, "Error in Input_Admin's response to start!\n");
		exit(0);
	}

	initscr();
	noecho();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE); //harmful, doesnt work.

	while(reply.type != END){
		int ch = getch();
		if(ch != -1){
			msg.type = KEYBOARD_INPUT; // send Input Admin the input
			msg.key = ch;
			if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
				fprintf(stderr, "Cannot notify Input_Admin the input keys!\n");
				exit(0);
			}
			if(!(reply.type == OKAY)){
				if(reply.type == END){
					continue;
				}
				else{
					fprintf(stderr, "Error in Input_Admin's response to input move!\n");
					exit(0);
				}
			}
		}
	}

	if(name_detach() == -1){
		fprintf(stderr, "Cannot detach name!\n");
		exit(0);
	}
}
