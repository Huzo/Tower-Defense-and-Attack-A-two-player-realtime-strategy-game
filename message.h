#ifndef __MESSAGE_H
#define MAX_UNIT 100

/* Type of Messages */
typedef enum{
	INIT, FAIL, END, OKAY,
	REGISTER_TIMER, TIMER_READY, SLEEP,
	REGISTER_COURIER, COURIER_READY, DISPLAY_ARENA,
	REGISTER_KEYBOARD, KEYBOARD_READY,
	REGISTER_HUMAN, HUMAN_READY, HUMAN_MOVE, UPDATE,
	PAINTER_READY, PAINT, KEYBOARD_INPUT, START
} MESSAGE_TYPE;

/* Type of Units */
typedef enum{
	MINE, HOPLITE, LANCER, WALL
} UNIT_TYPE;

/* Type of Timers */
typedef enum{
	MINE_TIMER, LANCER_TIMER, HOPLITE_TIMER
} TIMER_TYPE;

/* Force */
typedef enum{
	RED, BLUE
} FORCE;

/* Action */
typedef enum{
	NOACTION, MOVEEAST, MOVESOUTH, MOVEWEST, MOVENORTH,
	PLACELANCER, PLACEHOPLITE, UPDATEMINE, UPDATEWALL
} ACTION;

/* Coordinate */
typedef struct{
	int x,y;
} COORDINATE;

/* Player */
typedef struct{
	COORDINATE pos;
	FORCE force;
	int health;
	int resource;
	int unit_no;
} PLAYER;

/* Unit */
typedef struct{
	int active;
	int highlight;
	COORDINATE pos;
	FORCE force;
	UNIT_TYPE unit_type;
	int health;
} UNIT;

/* Arena */
typedef struct{
	UNIT units[2*MAX_UNIT];
	PLAYER players[2];
} ARENA;

/*Message Format*/
typedef struct{
	MESSAGE_TYPE type;
	TIMER_TYPE timer_type;
	int interval;
	ACTION act;
	ARENA arena;
	int key;
	int humanId;
} MESSAGE;

#endif