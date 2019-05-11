## ESTR 3106 – Principles of Programming Languages – Spring 2019

```
Assignment 1 Specification
Concurrent Programming (Administrator-and-Worker)
```
# 1 Introduction
![](https://github.com/Huzo/Tower-Defense-and-Attack-A-two-player-realtime-strategy-game/blob/master/Screenshot2.png)
In this assignment, you need to implement the real-time strategy game _Tower Defense and Attack_ ,
which is a simplified version of _Dominance_ (https://github.com/YangVincent/Domination). You
need to implement the game using the C programming langauge. For concurrency control, you
should use the Send/Receive/Reply (SRR) messaging primitives (first popularized in the commer-
cial real-time operating system, QNX) provided by the library, called Synchronous Interprocess
Messaging Project for LINUX (SIMPL) version 3.3.8. You should use the ncurses library to im-
plement the textual user interface.

# 2 Tower Defense and Attack

_Tower Defense and Attack_ is a two-player real-time strategy game played on a 20 by 100 arena.
Each of the two players, representing two forces **RED** and **BLUE** , occupies half (20 by 50) of the
arena as **territory**. The players need to place/build **units** at strategic spots in their respective
territories to defend their respective bases and attack each other. Each player has to manage his
resources named **Kyanite** , use resources to build units in the arena, and strategically send robots
to attack the opponent.
In the beginning, each player has 1000 **health points (HP)** , 500 Kyanites and no units in the
arena. The player will lose the game once his HP is reduced to zero, and the winner of the game
is the last survivor. A player’s HP is reduced when the player’s **baseline** , being the left edge or
the right edge of the arena, is reached by the opponent’s units. Placing units onto the arena costs
a player Kyanites. Different units have different costs, maximum HP, and their own functionality.
**Buildings** and **robots** are two types of units in this game. Buildings are unmoveable units. Each
player can either build a buildings on the empty territories or remove an existing buildings from
the terrories. The types of buildings include:

- A **Mine** generates resources. After being placed in the arena, a mine can generate 10 Kyanites
    for the player every second. The cost to build a mine is 200 Kyanites, and the maximum HP
    of mine is 200.
- A **Wall** can block friendly (in the same team) or hostile (in the opposite team) robots. The
    cost to build a wall is 20 Kyanites, and its maximum HP is 100.
Robots are moveable units which can cause damage to both hostile units and the opponent’s
HP. After being placed, it will take action (either move forward towards the opponent’s baseline
or attack hostile units) after a certain time interval, which is called **movement interval**. Differet
types of robots have different movement interval. The robots can **attack** hostile units, causing at
most its **melee damage (MD)** to the unit under attack. When the robot reaches the opponent’s
baseline, it will cause fixed **base damage (BD)** to the opponent’s HP. The rule of the robots’
movement will be explained later. Here we state the parameters for two types of robots as follows.
- A **Lancer** is a robot costing 20 Kyanites, and its maximum HP is 50. The movement interval
of a lancer is 0.2 seconds, and its MD and BD are 15 and 15 respectively.
- A **Hoplite** is yet another type of robot. Compared with a lancer, a hoplite has higher HP
and BD but lower MD. The cost of building a hoplite is 30 Kyanites, and its maximum HP
is 100. The movement interval is 0.3 seconds, and its MD and BD are 10 and 20 respectively.
All units belong to either RED or BLUE. When a unit is built, its HP is equal to its maximum
HP. Once a unit’s HP is reduced to zero, it will be eliminated from the arena. Each player could
at most placeM AX_U N ITunits on the arena.


Throughout the game, each player will have a **marker** indicating the current desired point
for the player to build units. The RED (BLUE) player can usew, s, aandd(↑,↓,←and→)
to change the position of the marker by one grid cell in the up, down, left and right directions
respectively. At the own marker posistion, a player can press appropriate keys to place units or
remove the player’s own units, if possible. The RED (BLUE) player can pressforg(jandk) to
place a lancer and a hoplite respectively in the desired point. As for buildings, pressingrort(u
ori) allows the RED (BLUE) player to place a mine or a wall respectively. Otherwise, the mine
or the wall occupying the position of the marker will be removed. There are several rules that we
should obey throughout this game:

1. All units should be placed within the arena, and the RED (BLUE) can only place units on
    their own territories. Some units may be represented by multiple characters (as specified in
    3.4), and every character should be within the arena.
2. All units in the arena cannot overlap with each other.
3. The buildings can be **removed** by a player, and half of its costs will be returned to the
    player.
4. For every movement interval, a robot can move forward if the cell in front is empty.
5. If a robot is blocked by a hostile unit, it will cause random damage ranging from 1 to its MD
    to the blocking unit.
6. If a robot is blocked by a friendly unit, the robot will wait until the friendly unit moves
    forward or is removed.
7. Once the robot reaches the opponent’s baseline, it disappears and the opponent’s HP is
    reduced by the robot’s base damage.

In this assignment, you need to write programs to implement the game with **keyboard input**.
You should use the **Administrator-and-Worker** model. In the following, we provide the process
design in Figure 1. You need to implement these processes in the design using the SRR messaging
primitives provided by the SIMPL library. Note that in **assignment 2** , you need to write an
interpreter of a **specification language** to implement an AI to play this game.

# 3 Assignment Details

## 3.1 Process Design

There are _seven_ types of processes in our design.

- Game_Adminmaintains the rules of the game and the current state of the game.
- Display_Admincontrols the screen output sequence.
- Timersleeps for a certain amount of time.
- Courierrelays the message between administration processes.
- Painterformats and paints the output to the screen.
- Input_Admincontrols the human controlled RED or BLUE players.
- Keyboardgets the human players’ input from the keyboard.

**You need to write a program for each of the seven major types of process:Game_Ad-
min,Display_Admin,Timer,Painter,Input_Admin,KeyboardandCourier**.
In the implementation, players RED and BLUE are human controlled. We useKeyboardto get
all the keys pressed and send toInput_Admin. We useCourier 0andCourier 1to relay the messages
fromInput_AdmintoGame_Adminfor the first and the second human player respectively.

![Figure 1](https://github.com/Huzo/Tower-Defense-and-Attack-A-two-player-realtime-strategy-game/blob/master/Screenshot.png)
```
Figure 1: The Administrator-and-Worker Architecture for the Game
```
There are three timer workers denoted byTimer 0toTimer 2.Timer 0is used to maintain the
time interval for Kyanite generation.Timer 1andTimer 2are used to maintain the time interval
for moving all Lancers and Hoplites respectively in the arena. Timerprocesses should take an
argument to distinguish their roles in the game.
All the screen output is handled by thePainterprocess, which keeps on getting paint jobs for
Display_Admin. The responsibility ofCourier 2is to relay the paint requests fromGame_Adminto
Display_Admin.
Before the game starts, we have to carry out the registration procedure. The first process to
start isGame_Admin. Next,TimerandCourier 2send registration requests toGame_Admin. Then,
Game_Adminreplies immediately whether the registration is successful or not. Registration fails
if theGame_Adminalready accepts enough number of processes.Courier 0andCourier 1have to
register withInput_Adminfirst. ThenCourier 0andCourier 1starts to relay the registration mes-
sages for human players fromInput_AdmintoGame_Admin. On starting the game,Game_Admin
asksTimerworkers to start the sleep job and replies the game start signal to human players.

## 3.2 Message types and Format
The message types and message formats are given in the header file “message.h”, which must
be included in your code. **You are not allowed to have any additional message type or
make any modification to the message format.**
The administrators and workers can use theMESSAGE.typefield to differentiate messages, and
can store/retrieve data in the necessary fields in theMESSAGEstructure with respect to the type
of the messages. For example, whenGame_Adminsends anINITmessage to aTimer,Game_Admin
only needs to specify thetype,timer_typeandintervalfields in theMESSAGEstructure. Thus,
when aTimerreceives a message, it can immediately sleep forintervalmicroseconds. We list in


appendix A the possible messages between the different processes and the fields used for each
message. **Message field usage must be strictly followed.**

## 3.3 Program Libraries

The concept of Administrator-and-Worker can be realized by the following primitives in the SIMPL
library:

```
int name_attach(char *processName, void (*exitFunc)());
int name_detach(void);
int name_locate(char *protocolName:hostName:processName);
```
```
int Send(int fd, void *out, void *in, unsigned outSize, unsigned inSize);
int Receive(char **ptr, void *inArea, unsigned maxBytes);
int Reply(char *ptr, void *outArea, unsigned size);
```
The API details can be found in the file$SIMPL HOME/docs/simpl-function.synopsiswhere
$SIMPL HOMEis the installation path of theSIMPLlibrary. You can download the pre-compiled
binaries of theSIMPLlibrary as a gzipped tar package from the course webpage. The details of
SIMPLproject and the sources of theSIMPLlibrary are available at:

```
http://www.icanprogram.com/simpl/
```
Description and sample programs on using theNCURSESlibrary are available at:

```
http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/
```
## 3.4 Output

For simplicity, we use the textual user interface based on the reference output format (using the
ncurseslibrary) as depicted in Figure 2.

![](https://github.com/Huzo/Tower-Defense-and-Attack-A-two-player-realtime-strategy-game/blob/master/Screenshot2.png)

```
Figure 2: Sample Screen Output
```
The reference output format is the MINIMUM requirement. The arena is a rectangle whose
size is 100 columns by 20 rows. The marker of each player is represented by a solid block of either

```
or. The mine is denoted by three characters “/0\” forming a 1 by 3 rectangle. Other units
```

will have different representations due to their forces. For example, the wall of red is “{”, while it
is “}” for blue. Similarly, lancers are denoted as “&/” and “\&”, and hoplites are denoted as “@]”
and “[@”. The upper part of the screen shows the name of this game, “Tower Defense and Attack”.
The lower part of the screen displays the state of players including their HP, Kyanites and unit
number. When the game ends, it will display the winner on the final screen shot. Note that some
units may be represented by multiple characters. The position of mine, lancer and hoplite is the
position of the character “0”, “&” and “@” respectively.
You are free to improve the reference output format (like using different colors). **Such im-
provement must be only in thePainterprocess and must obey strictly the messaging
interface.** Also, the following basic components must be included in each screen shot: the bound-
ary of the arena, the locations of different units, the state of players and the winner on the final
screen shot. In addition, the screen should be updated after every change of the mouse and cheese.
Of course, the output has to reflect the real situation of the arena by respecting the order of the
events.

# A Message Bewteen Processes

Messages betweenGame_AdminandTimer 0.. .Timer 2
REGISTER_TIMER -Timerregisters toGame_Admin. (Field: type)
INIT -Game_AdminconfirmsTimerregistration. (Fields: type, timer_type)
FAIL -Game_AdminfailsTimerregistration. (Field: type)
TIMER_READY -TimertellsGame_Adminthat it is ready. (Fields: type, timer_type)
SLEEP -Game_AdminasksTimerto sleep for a certain amount of time. (Fields: type, timer_type,
interval)
END -Game_Adminsends an ending message to Timer. (Field: type)

Messages betweenGame_AdminandCourier 2
REGISTER_COURIER -Courier 2registers toGame_Admin. (Field: type)
INIT -Game_AdminconfirmsCourier 2registration. (Fields: type)
FAIL -Game_AdminfailsCourier 2registration. (Field: type)
COURIER_READY -Courier 2notifiesGame_Adminthat it is ready. (Fields: type)
DISPLAY_ARENA -Game_AdminasksCourier 2to carry a display message. (Fields: type, arena)
OKAY -Courier 2notifiesGame_Adminthat the display message is delivered. (Fields: type)
END -Game_AdminasksCourier 2to deliver an ending message, and asksCourier 2to terminate
itself after delivering the message. (Fields: type, humanId, arena) (Here playerId denotes the
winner of the game)

Messages betweenDisplay_AdminandCourier 2
DISPLAY_ARENA -Courier 2notifiesDisplay_Adminthat there is a new display message. (Fields:
type, arena)
OKAY - Acknowledgement fromDisplay_Admin. (Field: type)
END -Courier 2delivers an ending message toDisplay_Admin

Messages betweenDisplay_AdminandPainter
PAINTER_READY-PainternotifiesDisplay_Adminthat it is ready to paint. (Field:type)
PAINT-Display_AdminasksPainterto paint the current game status. (Fields:type, arena)
END-Display_AdminasksPainterto paint the final screen, announce the winner, and terminate
itself. (Fields:type, humanId, arena)

Messages betweenGame_AdminandCourier 0 toCourier 1
REGISTER_HUMAN-CouriernotifiesGame_Adminof human player registration. (Field:type)
INIT-Game_Adminconfirms the human player registration. (Fields:type, humanId)
FAIL-Game_Adminfails the registration if all human players are already registered. (Field:type)
HUMAN_READY-CouriernotifiesGame_Adminthat the human player is ready. (Fields: type,
humanId)
START-Game_Admindeclares the game start. (Fields:type, arena)
HUMAN_MOVE-CouriernotifiesGame_Adminof human player’s request to take actions. (Fields:
type, humanId, act)
UPDATE-Game_Adminsends the current game status toCourier. (Fields:type, arena)
END-Game_Admingot a winner and terminates human player. (Fields:type, humanId)

Messages betweenInput_AdminandCourier 0 toCourier 1


REGISTER_COURIER-Courierregisters to theInput_Admin. (Field:type)
INIT-Input_Adminconfirms the registration. (Fields:type, humanId)
FAIL-Input_Adminfails the courier registration if all couriers are already present. (Field:type)
COURIER_READY-CouriernotifiesInput_Adminthat it is ready. (Fields:type, humanId)
REGISTER_HUMAN-Input_Adminasks the courier to carry a registration message. (Field:type)
INIT-CouriernotifiesInput_Adminthat the registration is successful. (Fields:type, humanId)
FAIL-CouriernotifiesInput_Adminthat the registration failed. (Fields:type, humanId)
HUMAN_READY-Input_AdminasksCourierto carry a ready signal for the human player. (Fields:
type, humanId)
START-CouriernotifiesInput_Adminthat the game start. (Fields:type, humanId, arena)
HUMAN_MOVES-Input_AdminasksCourierto carry a human player request to take action. (Fields:
type, humanId, act)
UPDATE-CouriernotifiesInput_Adminof the current game status. (Fields:type, humanId, arena)
END-CouriernotifiesInput_Adminthat the game ended. (Fields:type, humanId)

Messages betweenInput_AdminandKeyboard
REGISTER_KEYBOARD-Keyboardregisters toInput_Admin. (Field:type)
INIT-Input_AdmintellsKeyboardthe human ID for the list of human players. (Fields: type,
humanId)
KEYBOARD_READY-KeyboardnotifiesInput_Adminthat it is ready to start. (Field:type)
START-Input_AdmintellsKeyboardto start getting keys. (Field:type)
KEYBOARD_MOVES-KeyboardnotifiesInput_Adminwhen there is any key pressed. (Fields:type,
key)
OKAY- Acknowledgment fromInput_Admin. (Field:type)
END-Input_AdminnotifiesKeyboardthat the game ended. (Fields:type, humanId)
