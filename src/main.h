#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

// TODO: Create any necessary structs

/*
* For example, for a Snake game, one could be:
*
* struct snake {
*   int heading;
*   int length;
*   int row;
*   int col;
* };
*
* Example of a struct to hold state machine data:
*
* struct state {
*   int currentState;
*   int nextState;
* };
*
*/

// struct for the ball
struct ball {
    int x;
    int y;
    int velx;
    int vely;
};

// struct for the paddles
struct paddle {
    int x;
    int y;
};

#define BALL_SIZE 4         // size of the ball (diameter)
#define PADDLE_HEIGHT 20    // height of the paddles
#define PADDLE_WIDTH 2      // width of the paddles
#define PADDLE_REGION 8     // size of the top and bottom regions on the paddle for redirection
#define PADDLE_OFFSET 10    // how far from the walls the paddles spawn
#define PADDLE_SPEED_Y 3    // vertical speed of the paddles
#define PADDLE_SPEED_X 2    // horizontal speed of the paddles
#define BALL_SPEED 2        // base/min speed of the ball
#define SCORE_OFFSET_Y 10   // the vertical offset of the score displays
#define SCORE_OFFSET_X 20   // the horizontal offset of the score displays from the center 
#define RESET_TIME 60       // how many frames in between each round
#define REDIRECT_CHANCE 5   // chance out of 10 for a middle hit to change directions
#define MAX_VELY 5          // max vertical velocity of the ball
#define MAX_VELX 4          // max horizontal velocity of the ball

// base inaccuracy of the ai 
#define AI_ACCURACY (diff == 4 ? 0 : ((4 - diff) * 4 + 2))
// minimum distance from the center the ai can move at
// negative means on the side of player 1
#define AI_MOVETIME_MIN (diff == 4 ? -120 : ((diff - 1) * -20))
// maximum distance from the center the ai can move at
#define AI_MOVETIME (diff == 4 ? -120 : ((3 - diff) * 20))
// distance the ball is from the paddle before the ai starts moving right to hit it
#define AI_MOVEDIST_X (diff > 2 ? 50 : 0)
// distance the ball must be before the ai resets horizontally
#define AI_RESET_X (diff > 2 ? 60 : 0)

typedef int bool;
#define true 1
#define false 0

void run(u32 currentButtons);
bool calculate(u32 currentButtons);
void drawScore(int score, bool a, u32 color);
void redirect(int dirY, int dirX, int y);
void predict(void);

#endif
