CC=gcc
OS=_THISIS_LINUX
VER=_SIMPLIPC
SIMPL_DIR=${SIMPL_HOME}
INCL_DIR=${SIMPL_DIR}/include
CDFLAGS=-c -I ${INCL_DIR}
LDFLAGS=-L ${SIMPL_DIR}/lib -lsimpl
NCFLAGS=-lncurses
LIBS=
SOURCES=courier display_admin game_admin input_admin keyboard painter timer

all: ${SOURCES}

courier.o: message.h courier.c
	${CC} ${CDFLAGS} courier.c -o courier.o

courier: courier.o
	${CC} courier.o -o courier ${LDFLAGS}

display_admin.o: message.h display_admin.c
	${CC} ${CDFLAGS} display_admin.c -o display_admin.o

display_admin: display_admin.o
	${CC} display_admin.o -o display_admin ${LDFLAGS}

game_admin.o: message.h game_admin.c
	${CC} ${CDFLAGS} game_admin.c -o game_admin.o

game_admin: game_admin.o
	${CC} game_admin.o -o game_admin ${LDFLAGS} ${NCFLAGS}

input_admin.o: message.h input_admin.c
	${CC} ${CDFLAGS} input_admin.c -o input_admin.o

input_admin: input_admin.o
	${CC} input_admin.o -o input_admin ${LDFLAGS} ${NCFLAGS}

keyboard.o: message.h keyboard.c
	${CC} ${CDFLAGS} keyboard.c -o keyboard.o

keyboard: keyboard.o
	${CC} keyboard.o -o keyboard ${LDFLAGS} ${NCFLAGS}

painter.o: message.h painter.c
	${CC} ${CDFLAGS} painter.c -o painter.o

painter: painter.o
	${CC} painter.o -o painter ${LDFLAGS} ${NCFLAGS}

timer.o: message.h timer.c
	${CC} ${CDFLAGS} timer.c -o timer.o

timer: timer.o
	${CC} timer.o -o timer ${LDFLAGS}

# CLEAN UP

clean:
	@rm -f *.o *~ ${SOURCES}