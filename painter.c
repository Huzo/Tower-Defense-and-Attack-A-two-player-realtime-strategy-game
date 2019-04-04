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

void initBorders(WINDOW *inner_win);
void paintArena(ARENA arena, WINDOW *inner_win, WINDOW *outer_win);
void printEndGame(WINDOW *end_win, WINDOW *inner_win, WINDOW *outer_win, ARENA arena, int winnerId, int end_height, int end_width, int end_starty, int end_startx);

int main(){
	int fd;
	MESSAGE msg, reply;
	char name[] = "Painter";

	WINDOW *inner_win, *outer_win, *headerText_win, *end_win;

	int startx, starty, out_startx, out_starty, 
	text_startx, text_starty, width, height, out_width, 
	out_height, text_width, text_height, xMax, yMax,
	end_startx, end_starty, end_height, end_width;

	ARENA arena;

	int winnerId;

	initscr(); // start ncurses
	start_color();
	cbreak();
	noecho();

	curs_set(0); //disable cursor

	init_pair(1, COLOR_RED, COLOR_YELLOW);
	init_pair(10, COLOR_YELLOW, COLOR_RED);
	init_pair(2, COLOR_BLUE, COLOR_YELLOW);
	init_pair(20, COLOR_YELLOW, COLOR_BLUE);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_YELLOW);

	getmaxyx(stdscr, yMax, xMax);
	height = 22;
	width = 102;
	starty = (yMax / 2) - (height / 2);
	startx = (xMax / 2) - (width / 2);

	out_height = 26;
	out_width = 106;
	out_starty = (yMax / 2) - (out_height / 2);
	out_startx = (xMax / 2) - (out_width / 2);

	text_height = 1;
	text_width = 24;
	text_starty = starty - 1;
	text_startx = (xMax / 2) - (text_width / 2);

	end_height = 4;
	end_width = 35;
	end_starty = (yMax / 2) - (end_height / 2);
	end_startx = (xMax / 2) - (end_width / 2);

	if(name_attach(name, NULL) == -1){
		fprintf(stderr, "Cannot attach name!\n");
		exit(0);
	}

	if((fd = name_locate("Display_Admin")) == -1){
		fprintf(stderr, "Cannot locate name!\n");
		exit(0);
	}

	inner_win = newwin(height, width, starty, startx);
	outer_win = newwin(out_height, out_width, out_starty, out_startx);
	headerText_win = newwin(text_height, text_width, text_starty, text_startx);
	refresh();
	wbkgd(outer_win, COLOR_PAIR(1));
	wbkgd(headerText_win, COLOR_PAIR(3));
	wbkgd(inner_win, COLOR_PAIR(2));
	wattron(headerText_win, A_BLINK);
	mvwprintw(headerText_win, 0, 0, "Tower Defense and Attack");
	
	wrefresh(outer_win);
	wrefresh(inner_win);

	wattroff(headerText_win, A_BLINK);
	wrefresh(headerText_win);

	initBorders(inner_win);

	
	while(reply.type != END){
		msg.type = PAINTER_READY;
		if(Send(fd, &msg, &reply, sizeof(msg), sizeof(reply)) == -1){
			fprintf(stderr, "Cannot notify Display_Admin the readiness!\n");
			exit(0);
		}
		if(reply.type == PAINT){
			arena = reply.arena;
			paintArena(arena, inner_win, outer_win);
		}
		else if(reply.type == END){
			winnerId = reply.humanId;
			arena = reply.arena;
			paintArena(arena, inner_win, outer_win);
			printEndGame(end_win, inner_win, outer_win, arena, winnerId, end_height, end_width, end_starty, end_startx);
		}
	}

	if(name_detach() == -1){
		fprintf(stderr, "Cannot detach name!\n");
		exit(0);
	}

	endwin();
}

void initBorders(WINDOW *inner_win){
	int x = 0, y = 0;
	for(x = 0; x < 102; x++){
		for(y = 0; y < 22; y++){
			if(x <= 50){
				if(x == 0 || y == 0 || y == 21){
					wattron(inner_win, COLOR_PAIR(1));
					if((x == 0 && y == 0) || (x == 0 && y == 21)){
						mvwprintw(inner_win, y, x, "+");
					}
					else if(x == 0 || x == 101){
						mvwprintw(inner_win, y, x, "|");
					}
					else{
						mvwprintw(inner_win, y, x, "-");
					}
				}
				
			}
			else{
				if(x == 101 || y == 0 || y == 21){
					wattron(inner_win, COLOR_PAIR(2));
					if((x == 101 && y == 0) || (x == 101 && y == 21)){
						mvwprintw(inner_win, y, x, "+");
					}
					else if(x == 0 || x == 101){
						mvwprintw(inner_win, y, x, "|");
					}
					else{
						mvwprintw(inner_win, y, x, "-");
					}	
				}
			}
		}
	}
	wrefresh(inner_win);
}

void paintArena(ARENA arena, WINDOW *inner_win, WINDOW *outer_win){
	int i, j, y, x;

	wattron(inner_win, COLOR_PAIR(4)); // paint the board yellow first
	for(i = 1; i < 21; i++){ // y
		for(j = 1; j < 101; j++){ // x
			mvwprintw(inner_win, i, j, " ");
		}
	}

	for(i = 0; i < 2; i++){ // paint the units and live information
		wattron(inner_win, A_REVERSE);
		wattron(inner_win, A_BLINK);
		if(arena.players[i].force == RED){
			wattron(inner_win, COLOR_PAIR(10));
			wattron(outer_win, COLOR_PAIR(1));
			wattron(outer_win, A_BOLD);
			wattron(outer_win, A_BLINK);
			mvwprintw(outer_win, 24, 3, "Red $: %d/100000, health: %d, unit_no: %d/100",
				arena.players[i].resource, arena.players[i].health, arena.players[i].unit_no);
			wattroff(outer_win, A_BOLD);
			wattroff(outer_win, A_BLINK);

			for(j = 0; j < 2 * MAX_UNIT; j++){
				wattroff(inner_win, A_BLINK);
				if(arena.units[j].active == 1 && arena.units[j].force == RED){
					if(arena.units[j].highlight == 1){
						wattron(inner_win, A_BOLD);
					}
					switch(arena.units[j].unit_type){
						case MINE:
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x, "/");
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x + 1, "0");
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x + 2, "\\");
							wattroff(inner_win, A_BOLD);
							break;
						case HOPLITE:
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x + 1, "@");
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x + 2, "]");
							wattroff(inner_win, A_BOLD);
							break;
						case LANCER:
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x + 1, "&");
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x + 2, "/");
							wattroff(inner_win, A_BOLD);
							break;
						case WALL:
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x + 1, "{");
							wattroff(inner_win, A_BOLD);
							break;
					}
					wrefresh(inner_win);
				}
			}
			wrefresh(outer_win);
		}
		else{
			wattron(inner_win, COLOR_PAIR(20));
			wattron(outer_win, COLOR_PAIR(2));
			wattron(outer_win, A_BOLD);
			wattron(outer_win, A_BLINK);
			mvwprintw(outer_win, 24, 54, "Blue $: %d/100000, health: %d, unit_no: %d/100",
				arena.players[i].resource, arena.players[i].health, arena.players[i].unit_no);
			wattroff(outer_win, A_BOLD);
			wattroff(outer_win, A_BLINK);

			for(j = 0; j < 2 * MAX_UNIT; j++){
				wattroff(inner_win, A_BLINK);
				if(arena.units[j].active == 1 && arena.units[j].force == BLUE){
					if(arena.units[j].highlight == 1){
						wattron(inner_win, A_BOLD);
					}
					switch(arena.units[j].unit_type){
						case MINE:
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x, "/");
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x + 1, "0");
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x + 2, "\\");
							wattroff(inner_win, A_BOLD);
							break;
						case HOPLITE:
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x + 1, "@");
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x, "[");
							wattroff(inner_win, A_BOLD);
							break;
						case LANCER:
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x + 1, "&");
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x, "\\");
							wattroff(inner_win, A_BOLD);
							break;
						case WALL:
							mvwprintw(inner_win, arena.units[j].pos.y + 1, arena.units[j].pos.x + 1, "}");
							wattroff(inner_win, A_BOLD);
							break;
					}
					wrefresh(inner_win);
				}
			}
			wrefresh(outer_win);
		}
		wrefresh(outer_win);
		wrefresh(inner_win);

		if(arena.players[i].force == RED){ //paint the cursors
			wrefresh(inner_win);
			wattron(inner_win, A_REVERSE);
			wattron(inner_win, COLOR_PAIR(1));
			mvwprintw(inner_win, arena.players[i].pos.y + 1, arena.players[i].pos.x + 1, " ");
			wrefresh(inner_win);
			wattroff(inner_win, A_REVERSE);
		}
		else{
			wrefresh(inner_win);
			wattron(inner_win, A_REVERSE);
			wattron(inner_win, COLOR_PAIR(2));
			mvwprintw(inner_win, arena.players[i].pos.y + 1, arena.players[i].pos.x + 1, " ");
			wrefresh(inner_win);
			wattroff(inner_win, A_REVERSE);
		}
	}
}

void printEndGame(WINDOW *end_win, WINDOW *inner_win, WINDOW *outer_win, ARENA arena, int winnerId, int end_height, int end_width, int end_starty, int end_startx){
	end_win = newwin(end_height, end_width, end_starty, end_startx);
	wbkgd(end_win, COLOR_PAIR(3));
	paintArena(arena, inner_win, outer_win);
	if(winnerId == 0){
		mvwprintw(end_win, 0,0,"             GAME OVER!\n");
		wattron(end_win, A_BOLD);
		mvwprintw(end_win, 1,0,"         THE WINNER IS RED\n");
		wattroff(end_win, A_BOLD);
		mvwprintw(end_win, 2,0,"       PRESS ANY KEY TO EXIT\n");
		wattron(end_win, A_BLINK);
		mvwprintw(end_win, 3,0,"WINDOW WILL BE CLOSED IN 10 SECONDS");
		wattroff(end_win, A_BLINK);
	}
	else{
		mvwprintw(end_win, 0,0,"            GAME OVER!\n");
		wattron(end_win, A_BOLD);
		mvwprintw(end_win, 1,0,"         THE WINNER IS BLUE\n");
		wattroff(end_win, A_BOLD);
		mvwprintw(end_win, 2,0,"       PRESS ANY KEY TO EXIT\n");
		wattron(end_win, A_BLINK);
		mvwprintw(end_win, 3,0,"WINDOW WILL BE CLOSED IN 10 SECONDS");
		wattroff(end_win, A_BLINK);
	}
	wrefresh(end_win);
	int ch;
	ch = getch();
	while(ch != 'q'){ //dummy
		wrefresh(end_win);
		sleep(10);
		break;
	}
}