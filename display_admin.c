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

void enqueue(ARENA arena);
ARENA dequeue();

struct node{
    ARENA arena;
    struct node *next;
};

struct node *front = NULL; // queue for storing arenas.
struct node *rear = NULL;

int isGameOver = 0;
int canQuitNow = 0;

int main(){
	int target;
	char *fromWhom = NULL;
	MESSAGE msg, reply;
	char name[] = "Display_Admin";

	ARENA arena;

	int winnerId;

	if(name_attach(name, NULL) == -1){
		fprintf(stderr, "Cannot attach name!\n");
		exit(0);
	}

	while(!canQuitNow){
		if(Receive(&fromWhom, &msg, sizeof(msg)) == -1){
			fprintf(stderr, "Cannot receive message!\n");
			exit(0);
		}

		switch(msg.type){
			case DISPLAY_ARENA:
				arena = msg.arena;
				enqueue(arena);
				reply.type = OKAY;
				if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
					fprintf(stderr, "Cannot send OKAY to Courier!\n");
					exit(0);
				}
				break;

			case END:
				isGameOver = 1;
				winnerId = msg.humanId;
				reply.type = OKAY;
				if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
					fprintf(stderr, "Cannot send OKAY!\n");
					exit(0);
				}
				break;

			case PAINTER_READY:
				if(isGameOver){
					reply.type = END;
					reply.arena = arena;
					reply.humanId = winnerId;
					if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
						fprintf(stderr, "Cannot send END to Painter!\n");
						exit(0);
					}
					canQuitNow = 1;
					break;
				}
				reply.type = PAINT;
				if(front == NULL){
					reply.arena = arena;
				}
				else{
					reply.arena = dequeue();
				}
				if(Reply(fromWhom, &reply, sizeof(reply)) == -1){
					fprintf(stderr, "Cannot send PAINT to Painter!\n");
					exit(0);
				}
				break;
		}
	}

	if(name_detach() == -1){
		fprintf(stderr, "Cannot detach name!\n");
		exit(0);
	}

	return 0;
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