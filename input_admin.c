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

void whoPressedWhat(int key, int *whoPressed, int *whatIsPressed);
void enqueue_0(int whoPressed, int whoPressedWhat);
struct node *dequeue_0();
void enqueue_1(int whoPressed, int whoPressedWhat);
struct node *dequeue_1();

struct node{
    int values[2]; // first element for storing which player pressed, second element for the key pressed.
    struct node *next;
};

struct node *front_0 = NULL; // queue for storing commands from keyboard for player 0.
struct node *rear_0 = NULL;

struct node *front_1 = NULL; // queue for storing commands from keyboard for player 1.
struct node *rear_1 = NULL;

int isGameOver = 0;
int canQuitNow = 0;

int main(){
	int target;
	char *fromWhom = NULL;
	char *cour0 = NULL;
	char *cour1 = NULL;
	MESSAGE msg, reply;
	char name[] = "Input_Admin";
	int number_of_couriers_attached = 0;

	initscr();
	keypad(stdscr, TRUE);

	ARENA arena;

	if(name_attach(name, NULL) == -1){
		fprintf(stderr, "Cannot attach name!\n");
		exit(0);
	}

	int numberOfCouriersDeliveredENDmsg = 0;

	while(!canQuitNow){
		if(Receive(&fromWhom, &msg, sizeof(msg)) == -1){
			fprintf(stderr, "Cannot receive message!\n");
			exit(0);
		}

		switch(msg.type){
			case REGISTER_COURIER: // Couriers register to Input_Admin
				if(number_of_couriers_attached < 2){
					switch(number_of_couriers_attached){
						case 0:
							cour0 = fromWhom;
							break;
						case 1:
							cour1 = fromWhom;
							break;
					}
					number_of_couriers_attached++;
					reply.type = INIT;
					reply.humanId = number_of_couriers_attached - 1;
					if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
						fprintf(stderr, "Cannot reply to Courier_%d!\n",reply.humanId);
						exit(0);
					}
				}
				else{ // more than 2 couriers
					reply.type = FAIL;
					if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
						fprintf(stderr, "Cannot reply to Courier_%d!\n",number_of_couriers_attached - 1);
						exit(0);
					}
				}
				break;
			
			case COURIER_READY: // courier notifies that it is ready
				reply.type = REGISTER_HUMAN;
				if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
					fprintf(stderr, "Cannot reply to Courier_%d!\n",number_of_couriers_attached - 1);
					exit(0);
				}
				break;

			case INIT: // courier notifies registration is successful
				reply.type = HUMAN_READY;
				reply.humanId = msg.humanId;
				if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
					fprintf(stderr, "Cannot set human to ready!\n");
					exit(0);
				}
				break;

			case FAIL: // courier notifies that registration failed
				fprintf(stderr, "FAIL!\n");
				break;

			case START: // courier notifies that game started
				arena = msg.arena;

				if(msg.humanId == 0){
					if(front_0 == NULL){ // if queue is empty, send useless reply.
						reply.type = HUMAN_MOVE;
						reply.humanId = 0;
						reply.act = NOACTION;

						if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
							fprintf(stderr, "Cannot pass HUMAN_MOVE to courier!\n");
							exit(0);
						}
					}
					else{
						struct node *p;
						p = dequeue_0();

						reply.type = HUMAN_MOVE;
						reply.humanId = p->values[0];
						reply.act = (ACTION) p->values[1];
						free(p);

						if(Reply(cour0, &reply, sizeof(reply)) == -1){
							fprintf(stderr, "Cannot pass HUMAN_MOVE to courier!\n");
							exit(0);
						}
					}
				}
				else if(msg.humanId == 1){
					if(front_1 == NULL){ // if queue is empty, send useless reply.
						reply.type = HUMAN_MOVE;
						reply.humanId = 1;
						reply.act = NOACTION;

						if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
							fprintf(stderr, "Cannot pass HUMAN_MOVE to courier!\n");
							exit(0);
						}
					}
					else{
						struct node *p;
						p = dequeue_1();

						reply.type = HUMAN_MOVE;
						reply.humanId = p->values[0];
						reply.act = (ACTION) p->values[1];
						free(p);
						
						if(Reply(cour1, &reply, sizeof(reply)) == -1){
							fprintf(stderr, "Cannot pass HUMAN_MOVE to courier!\n");
							exit(0);
						}
					}
				}
				break;

			case UPDATE: // courier notifies of the current game state
				arena = msg.arena;

				if(msg.humanId == 0){
					if(front_0 == NULL){ // if queue is empty, send useless reply.
						reply.type = HUMAN_MOVE;
						reply.humanId = 0;
						reply.act = NOACTION;

						if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
							fprintf(stderr, "Cannot pass HUMAN_MOVE to courier!\n");
							exit(0);
						}
					}
					else{
						struct node *p;
						p = dequeue_0();

						reply.type = HUMAN_MOVE;
						reply.humanId = p->values[0];
						reply.act = (ACTION) p->values[1];
						free(p);

						if(Reply(cour0, &reply, sizeof(reply)) == -1){
							fprintf(stderr, "Cannot pass HUMAN_MOVE to courier!\n");
							exit(0);
						}
					}
				}
				else if(msg.humanId == 1){
					if(front_1 == NULL){ // if queue is empty, send useless reply.
						reply.type = HUMAN_MOVE;
						reply.humanId = 1;
						reply.act = NOACTION;

						if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
							fprintf(stderr, "Cannot pass HUMAN_MOVE to courier!\n");
							exit(0);
						}
					}
					else{
						struct node *p;
						p = dequeue_1();

						reply.type = HUMAN_MOVE;
						reply.humanId = p->values[0];
						reply.act = (ACTION) p->values[1];
						free(p);

						if(Reply(cour1, &reply, sizeof(reply)) == -1){
							fprintf(stderr, "Cannot pass HUMAN_MOVE to courier!\n");
							exit(0);
						}
					}
				}
				break;

			case END: // courier notifies that the game ended
				numberOfCouriersDeliveredENDmsg++;
				if(numberOfCouriersDeliveredENDmsg == 2){
					isGameOver = 1;
				}
				if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
					fprintf(stderr, "Cannot reply to Courier!\n");
					exit(0);
				}
				break;

			case REGISTER_KEYBOARD: // keyboard registers
				reply.type = INIT;
				if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
					fprintf(stderr, "Cannot reply to Keyboard!\n");
					exit(0);
				}
				break;

			case KEYBOARD_READY: // keyboard notifies that it is ready
				reply.type = START;
				if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
					fprintf(stderr, "Cannot reply to Keyboard!\n");
					exit(0);
				}
				break;

			case KEYBOARD_INPUT: // keyboard notifies that there is a key pressed
				if(isGameOver){
					canQuitNow = 1;

					reply.type = END;
					if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
						fprintf(stderr, "Cannot signal the END!\n");
						exit(0);
					}
					break;
				}

				int whoPressed;
				int whatIsPressed;
				whoPressedWhat(msg.key, &whoPressed, &whatIsPressed);
				if(whoPressed == 0){
					enqueue_0(whoPressed, whatIsPressed);
				}
				else{
					enqueue_1(whoPressed, whatIsPressed);
				}

				reply.type = OKAY;
				if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
					fprintf(stderr, "Cannot reply to Keyboard!\n");
					exit(0);
				}
				break;
		}
	}

	if(name_detach() == -1){
		fprintf(stderr, "Cannot detach name!\n");
		exit(0);
	}
}

void whoPressedWhat(int key, int *whoPressed, int *whatIsPressed){
	switch(key){
		case ((int) 'w'):
			*whoPressed = 0;
			*whatIsPressed = MOVENORTH;
			break;
		case ((int) 'a'):
			*whoPressed = 0;
			*whatIsPressed = MOVEWEST;
			break;
		case ((int) 's'):
			*whoPressed = 0;
			*whatIsPressed = MOVESOUTH;
			break;
		case ((int) 'd'):
			*whoPressed = 0;
			*whatIsPressed = MOVEEAST;
			break;
		case (KEY_UP):
			*whoPressed = 1;
			*whatIsPressed = MOVENORTH;
			break;
		case (KEY_LEFT):
			*whoPressed = 1;
			*whatIsPressed = MOVEWEST;
			break;
		case (KEY_DOWN):
			*whoPressed = 1;
			*whatIsPressed = MOVESOUTH;
			break;
		case (KEY_RIGHT):
			*whoPressed = 1;
			*whatIsPressed = MOVEEAST;
			break;
		case ((int) 'f'):
			*whoPressed = 0;
			*whatIsPressed = PLACELANCER;
			break;
		case ((int) 'g'):
			*whoPressed = 0;
			*whatIsPressed = PLACEHOPLITE;
			break;
		case ((int) 'j'):
			*whoPressed = 1;
			*whatIsPressed = PLACELANCER;
			break;
		case ((int) 'k'):
			*whoPressed = 1;
			*whatIsPressed = PLACEHOPLITE;
			break;
		case ((int) 'r'):
			*whoPressed = 0;
			*whatIsPressed = UPDATEMINE;
			break;
		case ((int) 't'):
			*whoPressed = 0;
			*whatIsPressed = UPDATEWALL;
			break;
		case ((int) 'u'):
			*whoPressed = 1;
			*whatIsPressed = UPDATEMINE;
			break;
		case ((int) 'i'):
			*whoPressed = 1;
			*whatIsPressed = UPDATEWALL;
			break;
	}
}

void enqueue_0(int whoPressed, int whoPressedWhat){
    struct node *nptr = malloc(sizeof(struct node));
    nptr->values[0] = whoPressed;
    nptr->values[1] = whoPressedWhat;
    nptr->next = NULL;
    if (rear_0 == NULL){
        front_0 = nptr;
        rear_0 = nptr;
    }
    else{
        rear_0->next = nptr;
        rear_0 = rear_0->next;
    }
}


struct node *dequeue_0(){
    if (front_0 == NULL){
        return NULL;
    }
    else if(front_0 == rear_0){
    	struct node *temp;
    	temp = front_0;
    	front_0 = NULL;
    	rear_0 = NULL;
    	return temp;
    }
    else{
        struct node *temp;
        temp = front_0;
        front_0 = front_0->next;
        return temp;
    }
}

void enqueue_1(int whoPressed, int whoPressedWhat){
    struct node *nptr = malloc(sizeof(struct node));
    nptr->values[0] = whoPressed;
    nptr->values[1] = whoPressedWhat;
    nptr->next = NULL;
    if (rear_1 == NULL){
        front_1 = nptr;
        rear_1 = nptr;
    }
    else{
        rear_1->next = nptr;
        rear_1 = rear_1->next;
    }
}


struct node *dequeue_1(){
    if (front_1 == NULL){
        return NULL;
    }
    else if(front_1 == rear_1){
    	struct node *temp;
    	temp = front_1;
    	front_1 = NULL;
    	rear_1 = NULL;
    	return temp;
    }
    else{
        struct node *temp;
        temp = front_1;
        front_1 = front_1->next;
        return temp;
    }
}