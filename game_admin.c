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

int isGameOver = 0;

void initArena(ARENA *arena);
void commitAction(ARENA *arena, int whichPlayer, int whatAction);
int moveUnits(TIMER_TYPE timer_type, ARENA *arena);
int isBlocked(ARENA *arena, int index);
int isEmpty(ARENA *arena, UNIT_TYPE unit_type, FORCE force);
void commitAttack(ARENA *arena, int index_atk, int index_hst);

void enqueue(ARENA arena);
ARENA dequeue();

struct node{
    ARENA arena; 
    struct node *next;
};

struct node *front = NULL; // queue for storing arenas.
struct node *rear = NULL;

int main(){
	srand(time(NULL));

	int target;
	char *fromWhom = NULL;
	char *cour2 = NULL;
	MESSAGE msg, reply;
	char name[] = "Game_Admin";
	int number_of_couriers_attached = 0;

	ARENA arena;

	initArena(&arena);
	enqueue(arena);

	if(name_attach(name, NULL) == -1){
		fprintf(stderr, "Cannot attach name!\n");
		exit(0);
	}

	TIMER_TYPE init_timers = 0; // types of timers
	int numberOfCouriersKilled = 0;
	int numberOfTimersKilled = 0;
	int winnerId;
	int isBufferEmpty = 0;

	while(numberOfCouriersKilled < 3 || numberOfTimersKilled < 3){
		if(arena.players[0].health <= 0 || arena.players[1].health <= 0){
			isGameOver = 1;
			if(arena.players[0].health <= 0){
				winnerId = 1;
			}
			else{
				winnerId = 0;
			}
		}

		if(Receive(&fromWhom, &msg, sizeof(msg)) == -1){
			fprintf(stderr, "Cannot receive message!\n");
			exit(0);
		}

		switch(msg.type){
			case REGISTER_TIMER: // init timer 0, timer 1 and timer 2
				reply.type = INIT;
				reply.timer_type = init_timers;
				init_timers++;
				if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
					fprintf(stderr, "Cannot reply to Timer_%d!\n",init_timers);
					exit(0);
				}
				break;

			case TIMER_READY:
				;
				if(isGameOver){
					reply.type = END;
					if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
						fprintf(stderr, "Cannot ask Timer_%d to end game!\n",init_timers);
						exit(0);
					}
					numberOfTimersKilled++;
					break;
				}

				int howManyMiliseconds = moveUnits(msg.timer_type, &arena); // move units and find how many seconds to sleep

				reply.type = SLEEP;
				reply.timer_type = msg.timer_type;
				reply.interval = howManyMiliseconds;
				if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
					fprintf(stderr, "Cannot ask Timer_%d to sleep!\n",init_timers);
					exit(0);
				}

				break;

			case REGISTER_HUMAN: // register courier_0 and courier_1
				if(number_of_couriers_attached < 2){
					number_of_couriers_attached++;
					reply.type = INIT;
					reply.humanId = number_of_couriers_attached - 1;
					if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
						fprintf(stderr, "Cannot reply to Courier_%d!\n",reply.humanId);
						exit(0);
					}
				}
				else{
					reply.type = FAIL;
					if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
						fprintf(stderr, "Cannot reply to Courier_%d!\n",reply.humanId);
						exit(0);
					}
				}
				break;

			case HUMAN_READY: // courier notifies that the human is ready
				reply.type = START;
				reply.arena = arena;
				if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
					fprintf(stderr, "Cannot start Courier!\n");
					exit(0);
				}
				break;

			case HUMAN_MOVE: // courier notifies that the human wants to take actions
				;
				if(isGameOver){
					reply.type = END;
					reply.arena = arena;
					reply.humanId = winnerId;
					if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
						fprintf(stderr, "Cannot signal the end!\n");
						exit(0);
					}
					numberOfCouriersKilled++;
					break;
				}

				int whoWantsAction = msg.humanId;
				int whatAction = msg.act;
				commitAction(&arena, whoWantsAction, whatAction);

				reply.type = UPDATE;
				reply.humanId = msg.humanId;
				reply.arena = arena;

				if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
					fprintf(stderr, "Cannot start inform update to courier!\n");
					exit(0);
				}

				break;

			case REGISTER_COURIER: // register courier_2
				reply.type = INIT;
				cour2 = fromWhom;
				if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
					reply.type = FAIL;
					if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
						fprintf(stderr, "Cannot reply to Courier_2!\n");
						exit(0);
					}
					fprintf(stderr, "Cannot reply to Courier_2!\n");
					exit(0);
				}
				break;

			case COURIER_READY:
				reply.type = DISPLAY_ARENA;
				if(front != NULL){
					reply.arena = dequeue();
				}
				else{
					isBufferEmpty = 1;
				}
				if(!isBufferEmpty){
					if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
						fprintf(stderr, "Cannot reply to Courier 2!\n");
						exit(0);
					}
				}
				break;

			case OKAY:
				if(isGameOver){
					reply.type = END;
					reply.arena = arena;
					reply.humanId = winnerId;
					if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
						fprintf(stderr, "Cannot signal the end!\n");
						exit(0);
					}
					numberOfCouriersKilled++;
					break;
				}

				reply.type = DISPLAY_ARENA;
				if(front != NULL){
					reply.arena = dequeue();
				}
				else{
					isBufferEmpty = 1;
				}
				if(!isBufferEmpty){
					if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
						fprintf(stderr, "Cannot reply to Courier 2\n");
						exit(0);
					}
				}
				break;
		}
		if(isBufferEmpty){
			if(front != NULL){
				isBufferEmpty = 0;
				if(Reply(cour2, &reply, sizeof(reply)) == -1){
					fprintf(stderr, "Cannot reply to Courier 2!\n");
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

void initArena(ARENA *arena){
	arena->players[0].force = RED;
	arena->players[0].pos.x = 24;
	arena->players[0].pos.y = 9;
	arena->players[0].health = 1000;
	arena->players[0].resource = 500;
	arena->players[0].unit_no = 0;

	arena->players[1].force = BLUE;
	arena->players[1].pos.x = 74;
	arena->players[1].pos.y = 9;
	arena->players[1].health = 1000;
	arena->players[1].resource = 500;
	arena->players[1].unit_no = 0;
}

void commitAction(ARENA *arena, int whichPlayer, int whatAction){
	FORCE force;
	if(whichPlayer == 0){
		force = RED;
	}
	else{
		force = BLUE;
	}

	switch(whatAction){
		int i, is_empty;

		case MOVEEAST:
			if(force == RED && arena->players[whichPlayer].pos.x < 49){
				arena->players[whichPlayer].pos.x++;
			}
			else if(force == BLUE && arena->players[whichPlayer].pos.x < 99){
				arena->players[whichPlayer].pos.x++;
			}
			break;
		case MOVESOUTH:
			if(arena->players[whichPlayer].pos.y < 19){
				arena->players[whichPlayer].pos.y++;
			}
			break;
		case MOVEWEST:
			if(force == RED && arena->players[whichPlayer].pos.x > 0){
				arena->players[whichPlayer].pos.x--;
			}
			else if(force == BLUE && arena->players[whichPlayer].pos.x > 50){
				arena->players[whichPlayer].pos.x--;
			}
			break;
		case MOVENORTH:
			if(arena->players[whichPlayer].pos.y > 0){
				arena->players[whichPlayer].pos.y--;
			}
			break;
		case PLACELANCER:
			if(force == RED){
				if(arena->players[force].pos.x == 49){
					break;
				}
			}
			else{
				if(arena->players[force].pos.x == 50){
					break;
				}
			}

			is_empty = isEmpty(arena, LANCER, force);

			if(!is_empty){
				break;
			}

			if(arena->players[force].resource >= 20 && arena->players[force].unit_no < 100){
				for(i = 0; i < 2 * MAX_UNIT; i++){
					if(arena->units[i].active == 0){
						arena->players[force].resource -= 20;
						arena->players[force].unit_no++;
						arena->units[i].active = 1;
						arena->units[i].force = force;
						arena->units[i].pos.x = arena->players[whichPlayer].pos.x;
						arena->units[i].pos.y = arena->players[whichPlayer].pos.y;
						arena->units[i].unit_type = LANCER;
						arena->units[i].health = 50;
						arena->units[i].highlight = 0;
						break;
					}
				}
			}
			break;
		case PLACEHOPLITE:
			if(force == RED){
				if(arena->players[force].pos.x == 49){
					break;
				}
			}
			else{
				if(arena->players[force].pos.x == 50){
					break;
				}
			}

			is_empty = isEmpty(arena, HOPLITE, force);

			if(!is_empty){
				break;
			}

			if(arena->players[force].resource >= 30 && arena->players[force].unit_no < 100){
				for(i = 0; i < 2 * MAX_UNIT; i++){
					if(arena->units[i].active == 0){
						arena->players[force].resource -= 30;
						arena->players[force].unit_no++;
						arena->units[i].active = 1;
						arena->units[i].force = force;
						arena->units[i].pos.x = arena->players[whichPlayer].pos.x;
						arena->units[i].pos.y = arena->players[whichPlayer].pos.y;
						arena->units[i].unit_type = HOPLITE;
						arena->units[i].health = 100;
						arena->units[i].highlight = 0;
						break;
					}
				}
			}
			break;
		case UPDATEMINE:
			;
			int mineExist = 0;
			for(i = 0; i < 2 * MAX_UNIT; i++){
				if(arena->units[i].unit_type == MINE && arena->units[i].active && arena->units[i].pos.y == arena->players[whichPlayer].pos.y &&
					(arena->units[i].pos.x == arena->players[whichPlayer].pos.x || arena->units[i].pos.x - 1 == arena->players[whichPlayer].pos.x 
					|| arena->units[i].pos.x + 1 == arena->players[whichPlayer].pos.x)){
					mineExist = 1;
					arena->units[i].active = 0;
					arena->players[force].resource += 100; //refund
					arena->players[force].unit_no--;
					if(arena->players[force].resource > 100000){
						arena->players[force].resource = 100000;
					}
					break;
				}
			}
			if(!mineExist){
				if(force == RED){
					if(arena->players[force].pos.x == 49 || arena->players[force].pos.x == 0){
						break;
					}
				}
				else{
					if(arena->players[force].pos.x == 50 || arena->players[force].pos.x == 99){
						break;
					}
				}

				is_empty = isEmpty(arena, MINE, force);

				if(!is_empty){
					break;
				}

				if(arena->players[force].resource >= 200 && arena->players[force].unit_no < 100){
					for (i = 0; i < 2 * MAX_UNIT; i++){
						if(arena->units[i].active == 0){
							arena->players[force].resource -= 200;
							arena->players[force].unit_no++;
							arena->units[i].active = 1;
							arena->units[i].force = force;
							arena->units[i].pos.x = arena->players[whichPlayer].pos.x;
							arena->units[i].pos.y = arena->players[whichPlayer].pos.y;
							arena->units[i].unit_type = MINE;
							arena->units[i].health = 200;
							arena->units[i].highlight = 0;
							break;
						}
					}
				}
			}
			break;
		case UPDATEWALL:
			;
			int wallExist = 0;
			for(i = 0; i < 2 * MAX_UNIT; i++){
				if(arena->units[i].unit_type == WALL && arena->units[i].active && arena->units[i].pos.x == arena->players[whichPlayer].pos.x && arena->units[i].pos.y == arena->players[whichPlayer].pos.y){
					wallExist = 1;
					arena->units[i].active = 0;
					arena->players[force].resource += 10; //refund
					arena->players[force].unit_no--;
					if(arena->players[force].resource > 100000){
						arena->players[force].resource = 100000;
					}
					break;
				}
			}
			if(!wallExist){
				is_empty = isEmpty(arena, WALL, force);

				if(!is_empty){
					break;
				}

				if(arena->players[force].resource >= 20 && arena->players[force].unit_no < 100){
					for (i = 0; i < 2 * MAX_UNIT; i++){
						if(arena->units[i].active == 0){
							arena->players[force].resource -= 20;
							arena->players[force].unit_no++;
							arena->units[i].active = 1;
							arena->units[i].force = force;
							arena->units[i].pos.x = arena->players[whichPlayer].pos.x;
							arena->units[i].pos.y = arena->players[whichPlayer].pos.y;
							arena->units[i].unit_type = WALL;
							arena->units[i].health = 100;
							arena->units[i].highlight = 0;
							break;
						}
					}
				}
			}
			break;
		case NOACTION:
			break;
	}
}

int moveUnits(TIMER_TYPE timer_type, ARENA *arena){
	int i;

	switch(timer_type){
		case MINE_TIMER:
			for(i = 0; i < 2 * MAX_UNIT; i++){
				if(arena->units[i].active && arena->units[i].unit_type == MINE){
					if(arena->units[i].force == RED){
						arena->players[RED].resource += 10;
						if(arena->players[RED].resource > 100000){
							arena->players[RED].resource = 100000;
						}
					}
					else{
						arena->players[BLUE].resource += 10;
						if(arena->players[BLUE].resource > 100000){
							arena->players[BLUE].resource = 100000;
						}
					}
					enqueue(*arena);
				}
			}
			return 1000;

		case LANCER_TIMER:
			for(i = 0; i < 2 * MAX_UNIT; i++){
				if(arena->units[i].active == 1 && arena->units[i].unit_type == LANCER){
					if(arena->units[i].force == RED){
						if(isBlocked(arena, i) == 1){
							//
						}
						else{
							arena->units[i].pos.x++;
							if(arena->units[i].pos.x >= 99){
								arena->units[i].active = 0;
								arena->players[RED].unit_no--;
								arena->players[BLUE].health -= 15;
								if(arena->players[BLUE].health <=0){
									isGameOver = 1;
								}
							}
						}
					}
					else{
						if(isBlocked(arena, i) == 1){
							//
						}
						else{
							arena->units[i].pos.x--;
							if(arena->units[i].pos.x <= 0){
								arena->units[i].active = 0;
								arena->players[BLUE].unit_no--;
								arena->players[RED].health -= 15;
								if(arena->players[RED].health <=0){
									isGameOver = 1;
								}
							}
						}
					}
				}
			}
			enqueue(*arena);
			return 200;

		case HOPLITE_TIMER:
			for(i = 0; i < 2 * MAX_UNIT; i++){
				if(arena->units[i].active == 1 && arena->units[i].unit_type == HOPLITE){
					if(arena->units[i].force == RED){
						if(isBlocked(arena, i) == 1){
							//
						}
						else{
							arena->units[i].pos.x++;
							if(arena->units[i].pos.x >= 99){
								arena->units[i].active = 0;
								arena->players[RED].unit_no--;
								arena->players[BLUE].health -= 15;
								if(arena->players[BLUE].health <=0){
									isGameOver = 1;
								}
							}
						}
					}
					else{
						if(isBlocked(arena, i) == 1){
							//
						}
						else{
							arena->units[i].pos.x--;
							if(arena->units[i].pos.x <= 0){
								arena->units[i].active = 0;
								arena->players[BLUE].unit_no--;
								arena->players[RED].health -= 20;
								if(arena->players[RED].health <=0){
									isGameOver = 1;
								}
							}
						}
					}
				}
			}
			enqueue(*arena);
			return 300;
	}
}

int isBlocked(ARENA *arena, int index){
	int x = arena->units[index].pos.x;
	int y = arena->units[index].pos.y;
	FORCE force = arena->units[index].force;

	int i;
	for(i = 0; i < 2 * MAX_UNIT; i++){
		if(arena->units[i].pos.y == y && arena->units[i].active == 1){
			if(arena->units[i].unit_type == MINE){
				if(force == RED){
					if(arena->units[i].pos.x - 3 == x){
						if(arena->units[i].force == BLUE){ // enemy
							commitAttack(arena, index, i);
						}
						else{
							arena->units[i].highlight = 0;
						}
						return 1;
					}
				}
				else{
					if(arena->units[i].pos.x + 3 == x){
						if(arena->units[i].force == RED){ // enemy
							commitAttack(arena, index, i);
						}
						else{
							arena->units[i].highlight = 0;
						}
						return 1;
					}
				}
				
			}
			else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
				if(force == RED){
					if(arena->units[i].force == BLUE){
						if(arena->units[i].pos.x - 3 == x){
							commitAttack(arena, index, i);
							return 1;
						}
					}
					else{
						if(arena->units[i].pos.x - 2 == x){
							arena->units[i].highlight = 0;
							return 1;
						}
					}
				}
				else{
					if(arena->units[i].force == RED){
						if(arena->units[i].pos.x + 3 == x){
							commitAttack(arena, index, i);
							return 1;
						}
					}
					else{
						if(arena->units[i].pos.x + 2 == x){
							arena->units[i].highlight = 0;
							return 1;
						}
					}
				}
			}
			else if(arena->units[i].unit_type == WALL){
				if(force == RED){
					if(arena->units[i].pos.x - 2 == x){
						if(arena->units[i].force == BLUE){ // enemy
							commitAttack(arena, index, i);
						}
						else{
							arena->units[i].highlight = 0;
						}
						return 1;
					}
				}
				else{
					if(arena->units[i].pos.x + 2 == x){
						if(arena->units[i].force == RED){ // enemy
							commitAttack(arena, index, i);
						}
						else{
							arena->units[i].highlight = 0;
						}
						return 1;
					}
				}
			}
		}
	}
	enqueue(*arena);
	return 0;
}

int isEmpty(ARENA *arena, UNIT_TYPE unit_type, FORCE force){
	int x = arena->players[force].pos.x;
	int y = arena->players[force].pos.y;
	int i;

	switch(unit_type){
		case MINE:
			for(i = 0; i < 2 * MAX_UNIT; i++){
				if(arena->units[i].pos.y == y && arena->units[i].active == 1){
					if(force == RED){
						if(arena->units[i].force == RED){ //ally
							if(arena->units[i].unit_type == WALL){
								if(arena->units[i].pos.x == x || arena->units[i].pos.x == x - 1 || arena->units[i].pos.x == x + 1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= -2){
									return 0;
								}
							}
						}
						else{ //enemy
							if(arena->units[i].unit_type == WALL){
								if(arena->units[i].pos.x == x || arena->units[i].pos.x == x - 1 || arena->units[i].pos.x == x + 1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 2 && arena->units[i].pos.x - x >= -1){
									return 0;
								}
							}
						}
					}
					else{
						if(arena->units[i].force == BLUE){ //ally
							if(arena->units[i].unit_type == WALL){
								if(arena->units[i].pos.x == x || arena->units[i].pos.x == x - 1 || arena->units[i].pos.x == x + 1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 2 && arena->units[i].pos.x - x >= -1){
									return 0;
								}
							}
						}
						else{ //enemy
							if(arena->units[i].unit_type == WALL){
								if(arena->units[i].pos.x == x || arena->units[i].pos.x == x - 1 || arena->units[i].pos.x == x + 1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= -2){
									return 0;
								}
							}
						}
					}
				}
			}
			return 1;

		case WALL:
			for(i = 0; i < 2 * MAX_UNIT; i++){
				if(arena->units[i].pos.y == y && arena->units[i].active == 1){
					if(force == RED){
						if(arena->units[i].force == RED){ //ally
							if(arena->units[i].unit_type == MINE){
								if(arena->units[i].pos.x == x || arena->units[i].pos.x == x - 1 || arena->units[i].pos.x == x + 1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 0 && arena->units[i].pos.x - x >= -1){
									return 0;
								}
							}
						}
						else{ //enemy
							if(arena->units[i].unit_type == MINE){
								if(arena->units[i].pos.x == x || arena->units[i].pos.x == x - 1 || arena->units[i].pos.x == x + 1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= 0){
									return 0;
								}
							}
						}
					}
					else{
						if(arena->units[i].force == BLUE){ //ally
							if(arena->units[i].unit_type == MINE){
								if(arena->units[i].pos.x == x || arena->units[i].pos.x == x - 1 || arena->units[i].pos.x == x + 1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= 0){
									return 0;
								}
							}
						}
						else{ //enemy
							if(arena->units[i].unit_type == MINE){
								if(arena->units[i].pos.x == x || arena->units[i].pos.x == x - 1 || arena->units[i].pos.x == x + 1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 0 && arena->units[i].pos.x - x >= -1){
									return 0;
								}
							}
						}
					}
				}
			}
			return 1;

		case HOPLITE:
			for(i = 0; i < 2 * MAX_UNIT; i++){
				if(arena->units[i].pos.y == y && arena->units[i].active == 1){
					if(force == RED){
						if(arena->units[i].force == RED){ //ally
							if(arena->units[i].unit_type == MINE){
								if(arena->units[i].pos.x - x <= 2 && arena->units[i].pos.x - x >= -1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == WALL){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= 0){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= 0){
									return 0;
								}
							}
						}
						else{ //enemy
							if(arena->units[i].unit_type == MINE){
								if(arena->units[i].pos.x - x <= 2 && arena->units[i].pos.x - x >= -1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == WALL){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= 0){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 2 && arena->units[i].pos.x - x >= 0){
									return 0;
								}
							}
						}
					}
					else{
						if(arena->units[i].force == BLUE){ //ally
							if(arena->units[i].unit_type == MINE){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= -2){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == WALL){
								if(arena->units[i].pos.x - x <= 0 && arena->units[i].pos.x - x >= -1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= -1){
									return 0;
								}
							}
						}
						else{ //enemy
							if(arena->units[i].unit_type == MINE){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= -2){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == WALL){
								if(arena->units[i].pos.x - x <= 0 && arena->units[i].pos.x - x >= -1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 0 && arena->units[i].pos.x - x >= -2){
									return 0;
								}
							}
						}
					}
				}
			}
			return 1;

		case LANCER:
			for(i = 0; i < 2 * MAX_UNIT; i++){
				if(arena->units[i].pos.y == y && arena->units[i].active == 1){
					if(force == RED){
						if(arena->units[i].force == RED){ //ally
							if(arena->units[i].unit_type == MINE){
								if(arena->units[i].pos.x - x <= 2 && arena->units[i].pos.x - x >= -1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == WALL){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= 0){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= 0){
									return 0;
								}
							}
						}
						else{ //enemy
							if(arena->units[i].unit_type == MINE){
								if(arena->units[i].pos.x - x <= 2 && arena->units[i].pos.x - x >= -1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == WALL){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= 0){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 2 && arena->units[i].pos.x - x >= 0){
									return 0;
								}
							}
						}
					}
					else{
						if(arena->units[i].force == BLUE){ //ally
							if(arena->units[i].unit_type == MINE){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= -2){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == WALL){
								if(arena->units[i].pos.x - x <= 0 && arena->units[i].pos.x - x >= -1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= -1){
									return 0;
								}
							}
						}
						else{ //enemy
							if(arena->units[i].unit_type == MINE){
								if(arena->units[i].pos.x - x <= 1 && arena->units[i].pos.x - x >= -2){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == WALL){
								if(arena->units[i].pos.x - x <= 0 && arena->units[i].pos.x - x >= -1){
									return 0;
								}
							}
							else if(arena->units[i].unit_type == HOPLITE || arena->units[i].unit_type == LANCER){
								if(arena->units[i].pos.x - x <= 0 && arena->units[i].pos.x - x >= -2){
									return 0;
								}
							}
						}
					}
				}
			}
			return 1;
	}
}

void commitAttack(ARENA *arena, int index_atk, int index_hst){
	UNIT_TYPE attacker_type = arena->units[index_atk].unit_type;
	UNIT_TYPE hostile_type = arena->units[index_hst].unit_type;

	int md;

	if(attacker_type == LANCER){
		md = 15;
	}
	else{
		md = 10;
	}

	if(arena->units[index_atk].health > 0 && arena->units[index_hst].health > 0){
		int rand_atk = (rand() % md)  + 1;
		arena->units[index_hst].health -= rand_atk;
		arena->units[index_hst].highlight = 1;
		arena->units[index_atk].highlight = 0;
		if(arena->units[index_hst].health <= 0){
			arena->units[index_hst].active = 0;
			arena->units[index_hst].highlight = 0;
			arena->players[arena->units[index_hst].force].unit_no--;
		}
		enqueue(*arena);
	}
}

void enqueue(ARENA arena){
    struct node *nptr = malloc(sizeof(struct node));
    int i;
	//copy content
	for(i = 0; i < 2; i++){
		nptr->arena.players[i].unit_no = arena.players[i].unit_no;
		nptr->arena.players[i].resource = arena.players[i].resource;
		nptr->arena.players[i].health = arena.players[i].health;
		nptr->arena.players[i].pos.x = arena.players[i].pos.x;
		nptr->arena.players[i].pos.y = arena.players[i].pos.y;
		nptr->arena.players[i].force = arena.players[i].force;
	}
	for(i = 0; i < 2 * MAX_UNIT; i++){
		nptr->arena.units[i].active = arena.units[i].active;
		nptr->arena.units[i].highlight = arena.units[i].highlight;
		nptr->arena.units[i].health = arena.units[i].health;
		nptr->arena.units[i].pos.x = arena.units[i].pos.x;
		nptr->arena.units[i].pos.y = arena.units[i].pos.y;
		nptr->arena.units[i].force = arena.units[i].force;
		nptr->arena.units[i].unit_type = arena.units[i].unit_type;
	}
    nptr->next = NULL;
    if (rear == NULL){
        front = nptr;
        rear = nptr;
    }
    else{
        rear->next = nptr;
        rear = rear->next;
    }
}


ARENA dequeue(){
	struct node *temp;
	temp = front;
	ARENA arena_return;

	int i;
	//copy content
	for(i = 0; i < 2; i++){
		arena_return.players[i].unit_no = temp->arena.players[i].unit_no;
		arena_return.players[i].resource = temp->arena.players[i].resource;
		arena_return.players[i].health = temp->arena.players[i].health;
		arena_return.players[i].pos.x = temp->arena.players[i].pos.x;
		arena_return.players[i].pos.y = temp->arena.players[i].pos.y;
		arena_return.players[i].force = temp->arena.players[i].force;
	}
	for(i = 0; i < 2 * MAX_UNIT; i++){
		arena_return.units[i].active = temp->arena.units[i].active;
		arena_return.units[i].highlight = temp->arena.units[i].highlight;
		arena_return.units[i].health = temp->arena.units[i].health;
		arena_return.units[i].pos.x = temp->arena.units[i].pos.x;
		arena_return.units[i].pos.y = temp->arena.units[i].pos.y;
		arena_return.units[i].force = temp->arena.units[i].force;
		arena_return.units[i].unit_type = temp->arena.units[i].unit_type;
	}

    if(front == rear){
    	front = NULL;
    	rear = NULL;
    	free(temp);
    	return arena_return;
    }
    else{
        front = front->next;
        free(temp);
        return arena_return;
    }
}