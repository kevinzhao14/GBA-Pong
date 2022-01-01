#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gba.h"
#include "images/bg.h"              // background image for the play screen
#include "images/menuSingle.h"      // menu w/ singleplayer selected
#include "images/menuTwo.h"         // menu w/ two player selected
#include "images/paddles.h"         // image for the paddles
#include "images/balls.h"           // image for the ball
#include "images/diffEasy.h"        // difficulty menu w/ easy selected
#include "images/diffNormal.h"      // difficulty menu w/ normal selected
#include "images/diffHard.h"        // difficulty menu w/ hard selected
#include "images/diffImpossible.h"  // difficulty menu w/ impossible selected

enum gba_state {
  START,
  MENU,
  DIFF,
  LOAD,
  RESET,
  PLAY,
  PAUSE
};

static enum gba_state state = START;      // state of the game

static struct ball ball = {0, 0, 0, 0};   // ball 
static struct paddle paddle1 = {0, 0};    // right paddle
static struct paddle paddle2 = {0, 0};    // left paddle

static int score1 = 0;    // score of player 1 (right-side);
static int score2 = 0;    // score of player 2 (left-side)

static u32 startFrame = 0;    // the # of the total frame for the game to start at
static bool start1 = true;    // which side to serve to

static int predictedY = 0;    // the predicted Y position on contact by the AI
static int moveTime = 0;      // when the AI will start moving to hit the ball

// 0 = none   1 = singleplayer    2 = multiplayer
static int gamemode = 0;
// 1 = easy   2 = normal    3 = hard    4 = impossible
static int diff = 0;

int main(void) {
  // Manipulate REG_DISPCNT here to set Mode 3. //
  REG_DISPCNT = MODE3 | BG2_ENABLE;

  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  while (1) {
    currentButtons = BUTTONS; // Load the current state of the buttons
    switch (state) {
      case START:
        wait();

        // load menu screen
        state = MENU;
        gamemode = 1;
        drawFullScreenImageDMA(menuSingle);
        break;
      case MENU:
        wait();

        // if pressing start, go to difficulty screen for singleplayer or play screen for two player
        if (!KEY_DOWN(BUTTON_START, previousButtons) && KEY_DOWN(BUTTON_START, currentButtons)) {
          if (gamemode == 1) {
            drawFullScreenImageDMA(diffEasy);
            diff = 1;
            state = DIFF;
          } else {
            state = LOAD;
          }
        }

        // selecting game modes using arrow keys
        if (gamemode == 1 && KEY_DOWN(BUTTON_DOWN, currentButtons)) {
          gamemode = 2;
          drawFullScreenImageDMA(menuTwo);
        }
        if (gamemode == 2 && KEY_DOWN(BUTTON_UP, currentButtons)) {
          gamemode = 1;
          drawFullScreenImageDMA(menuSingle);
        }
        break;
      case DIFF:
        wait();

        // start game
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
          state = LOAD;
          break;
        }
        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
          gamemode = 1;
          drawFullScreenImageDMA(menuSingle);
          state = MENU;
          break;
        }

        // selecting difficulty using arrow keys
        bool sw = false;
        if (diff < 4 && KEY_JUST_PRESSED(BUTTON_DOWN, currentButtons, previousButtons)) {
          diff++;
          sw = true;
        } else if (diff > 1 && KEY_JUST_PRESSED(BUTTON_UP, currentButtons, previousButtons)) {
          diff--;
          sw = true;
        }
        if (sw) {
          switch(diff) {
            case 1:
              drawFullScreenImageDMA(diffEasy);
              break;
            case 2:
              drawFullScreenImageDMA(diffNormal);
              break;
            case 3:
              drawFullScreenImageDMA(diffHard);
              break;
            case 4:
              drawFullScreenImageDMA(diffImpossible);
              break;
            default:
              break;
          }
        }
        break;
      case LOAD:
        wait();

        // reset paddle positions
        paddle1.x = 240 - PADDLE_OFFSET - PADDLE_WIDTH;
        paddle1.y = (160 - PADDLE_HEIGHT) / 2;
        paddle2.x = PADDLE_OFFSET;
        paddle2.y = (160 - PADDLE_HEIGHT) / 2;

        // reset scores
        score2 = 0;
        score1 = 0;

        // reset serve side
        start1 = true;

        // draw play screen background
        drawFullScreenImageDMA(bg2);
        state = RESET;
        break;
      case RESET:
        // reset ball position
        ball.x = (240 - BALL_SIZE) / 2;
        ball.y = (160 - BALL_SIZE) / 2;
        ball.velx = 0;
        ball.vely = 0;

        // calculate when to start the round
        startFrame = vBlankCounter + RESET_TIME;

        // reset AI vars
        moveTime = 119;
        predictedY = 79;

        // start
        state = PLAY;
        break;
      case PLAY:
        // wait for VDraw only, so we can do calculations while the screen draws the previous frame. this saves some time
        // so that we can focus on drawing during VBlank.
        waitForVDraw();

        // reset game
        if (KEY_DOWN(BUTTON_SELECT, currentButtons)) {
          state = START;
          break;
        }

        // pause game
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
          drawString(67, 101, "PAUSED", WHITE);
          drawString(84, 53, "Press START to resume.", WHITE);
          state = PAUSE;
          break;
        }

        // start the round
        if (vBlankCounter == startFrame) {
          // generate ball velocity
          ball.velx = (start1 ? 1 : -1) * BALL_SPEED;
          ball.vely = 0;

          // predict for the AI if serving to it
          if (!start1) {
            predict();
          }
        }

        // run the game cycle
        run(currentButtons);
        break;
      case PAUSE:
        // resume game
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
          drawFullScreenImageDMA(bg2);
          state = PLAY;
          break;
        }

        // reset game
        if (KEY_DOWN(BUTTON_SELECT, currentButtons)) {
          state = START;
        }
        break;
      default:
        break;
    }

    previousButtons = currentButtons; // Store the current state of the buttons
  }

  return 0;
}

void run(u32 currentButtons) {
  // save previous ball positions to erase with
  int ballx = ball.x;
  int bally = ball.y;

  int paddle1x = paddle1.x;
  int paddle1y = paddle1.y;
  int paddle2x = paddle2.x;
  int paddle2y = paddle2.y;


  // calculate stuff
  bool calc = calculate(currentButtons);

  // wait for vblank to start drawing stuff
  waitForVBlank();
  
  // erase old ball
  drawRectDMA(bally, ballx, BALL_SIZE, BALL_SIZE, BLACK);

  // reset if someone loses, otherwise, draw the new ball position
  if (!calc) {
    state = RESET;
  } else {
    drawImageDMA(ball.y, ball.x, BALL_SIZE, BALL_SIZE, balls);
  }
  
  // erase old paddle vertical pixels
  if (paddle1.y > paddle1y) {
    drawRectDMA(paddle1y, paddle1x, PADDLE_WIDTH, paddle1.y - paddle1y, BLACK);
  } else if (paddle1.y < paddle1y) {
    drawRectDMA(paddle1.y + PADDLE_HEIGHT, paddle1x, PADDLE_WIDTH, paddle1y - paddle1.y, BLACK);
  }
  if (paddle2.y > paddle2y) {
    drawRectDMA(paddle2y, paddle2x, PADDLE_WIDTH, paddle2.y - paddle2y, BLACK);
  } else if (paddle2.y < paddle2y) {
    drawRectDMA(paddle2.y + PADDLE_HEIGHT, paddle2x, PADDLE_WIDTH, paddle2y - paddle2.y, BLACK);
  }

  // erase old paddle horizontal pixels
  if (paddle1.x > paddle1x) {
    drawRectDMA(paddle1y, paddle1x, paddle1.x - paddle1x, PADDLE_HEIGHT, BLACK);
  } else if (paddle1.x < paddle1x) {
    drawRectDMA(paddle1y, paddle1.x + PADDLE_WIDTH, paddle1x - paddle1.x, PADDLE_HEIGHT, BLACK);
  }
  if (paddle2.x > paddle2x) {
    drawRectDMA(paddle2y, paddle2x, paddle2.x - paddle2x, PADDLE_HEIGHT, BLACK);
  } else if (paddle2.x < paddle2x) {
    drawRectDMA(paddle2y, paddle2.x + PADDLE_WIDTH, paddle2x - paddle2.x, PADDLE_HEIGHT, BLACK);
  }

  // draw paddles
  drawImageDMA(paddle1.y, paddle1.x, PADDLE_WIDTH, PADDLE_HEIGHT, paddles);
  drawImageDMA(paddle2.y, paddle2.x, PADDLE_WIDTH, PADDLE_HEIGHT, paddles);

  // draw scores
  drawScore(score1, true, WHITE);
  drawScore(score2, false, WHITE);
}

void drawScore(int score, bool a, u32 color) {
  char scoreString[3];
  sprintf(scoreString, "%d", score);
  if (a) {
    drawString(SCORE_OFFSET_Y, 120 + strlen(scoreString) * 6 + SCORE_OFFSET_X, scoreString, color);
  } else {
    drawString(SCORE_OFFSET_Y, 119 - strlen(scoreString) * 6 - SCORE_OFFSET_X, scoreString, color);
  }
}

bool calculate(u32 currentButtons) {
  // whether the paddle has moved up/down and left/right
  int moveAy = 0;
  int moveAx = 0;
  if (KEY_DOWN(BUTTON_UP, currentButtons)) {
    paddle1.y -= PADDLE_SPEED_Y;
    moveAy = 1;
  }
  if (KEY_DOWN(BUTTON_DOWN, currentButtons)) {
    paddle1.y += PADDLE_SPEED_Y;
    moveAy = -1;
  }
  if (KEY_DOWN(BUTTON_RIGHT, currentButtons)) {
    paddle1.x += PADDLE_SPEED_X;
    moveAx = -1;
  }
  if (KEY_DOWN(BUTTON_LEFT, currentButtons)) {
    paddle1.x -= PADDLE_SPEED_X;
    moveAx = 1;
  }  
  
  // keep paddle in-bounds
  if (paddle1.y < 0) {
    paddle1.y = 0;
    moveAy = 0;
  } else if (paddle1.y + PADDLE_HEIGHT > 160) {
    paddle1.y = 160 - PADDLE_HEIGHT;
    moveAy = 0;
  }
  if (paddle1.x + PADDLE_WIDTH > 240) {
    paddle1.x = 240 - PADDLE_WIDTH;
    moveAx = 0;
  } else if (paddle1.x < 120 + BALL_SIZE) {
    paddle1.x = 120 + BALL_SIZE;
    moveAx = 0;
  }

  // same as ^ but for the left-paddle
  int moveBy = 0;
  int moveBx = 0;
  if (gamemode == 2) {  // if two-player, allow player to move paddle
    if (KEY_DOWN(BUTTON_A, currentButtons)) {
      paddle2.y -= PADDLE_SPEED_Y;
      moveBy = 1;
    }
    if (KEY_DOWN(BUTTON_B, currentButtons)) {
      paddle2.y += PADDLE_SPEED_Y;
      moveBy = -1;
    }
    if (KEY_DOWN(BUTTON_L, currentButtons)) {
      paddle2.x -= PADDLE_SPEED_X;
      moveBx = -1;
    }
    if (KEY_DOWN(BUTTON_R, currentButtons)) {
      paddle2.x += PADDLE_SPEED_X;
      moveBx = 1;
    }
  } else if (gamemode == 1) { // otherwise, let AI move the paddles
    // if the ball is within range, we can move the paddle to position
    if (ball.x <= moveTime) {
      // move up
      if (paddle2.y + PADDLE_HEIGHT / 2 > predictedY + PADDLE_SPEED_Y / 2) {
        paddle2.y -= PADDLE_SPEED_Y;
        moveBy = 1;
        
        // move down
      } else if (paddle2.y + PADDLE_HEIGHT / 2 < predictedY - PADDLE_SPEED_Y / 2) {
        paddle2.y += PADDLE_SPEED_Y;
        moveBy = -1;
      }
    }
    // if the ball is within horizontal range, move towards it to hit
    if (ball.x <= paddle2.x + PADDLE_WIDTH + AI_MOVEDIST_X && ball.velx < 0) {
      paddle2.x += PADDLE_SPEED_X;
      moveBx = 1;

      // if the ball is out of range, reset back horizontally
    } else if ((ball.x > paddle2.x + PADDLE_WIDTH + AI_RESET_X || ball.velx == 0) && paddle2.x > PADDLE_OFFSET) {
      paddle2.x -= PADDLE_SPEED_X;
      moveBx = -1;
    }
  }
  if (paddle2.y < 0) {
    paddle2.y = 0;
    moveBy = 0;
  } else if (paddle2.y + PADDLE_HEIGHT > 160) {
    paddle2.y = 160 - PADDLE_HEIGHT;
    moveBy = 0;
  }
  if (paddle2.x < 0) {
    paddle2.x = 0;
    moveBx = 0;
  } else if (paddle2.x + PADDLE_WIDTH > 119 - BALL_SIZE) {
    paddle2.x = 119 - BALL_SIZE - PADDLE_WIDTH;
    moveBx = 0;
  }

  // move the ball
  ball.x += ball.velx;
  ball.y += ball.vely;

  // border collisions
  if (ball.y < 0) {
    ball.y = -ball.y;
    ball.vely = - ball.vely;
  } else if (ball.y + BALL_SIZE > 159) {
    ball.y = (ball.y < 160 ? 318 : 320) - ball.y - 2 * BALL_SIZE;
    ball.vely = -ball.vely;
  }

  // paddle collisions
  if (ball.velx > 0 
      && ball.x <= paddle1.x + PADDLE_WIDTH && ball.x + BALL_SIZE >= paddle1.x 
      && ball.y <= paddle1.y + PADDLE_HEIGHT && ball.y + BALL_SIZE >= paddle1.y) {
    ball.x = paddle1.x - (ball.x + BALL_SIZE - paddle1.x);
    redirect(moveAy, moveAx, paddle1.y);
    ball.velx = -ball.velx;
    if (gamemode == 1) { // allow AI to predict of player 1 hit and are in singleplayer
      predict();
    }
  } else if (ball.velx < 0 
      && ball.x <= paddle2.x + PADDLE_WIDTH && ball.x + BALL_SIZE >= paddle2.x 
      && ball.y <= paddle2.y + PADDLE_HEIGHT && ball.y + BALL_SIZE >= paddle2.y) {
    ball.x = 2 * (paddle2.x + PADDLE_WIDTH) - ball.x;
    ball.velx = -ball.velx;
    redirect(moveBy, moveBx, paddle2.y);
  }

  // left/right side wall collisions - increment scores
  if (ball.x < 0) {
    // erase old score
    drawScore(score1, true, BLACK);
    score1++; // increment score
    start1 = false; // end round/restart
    return false;
  } else if (ball.x > 239) {
    drawScore(score2, false, BLACK);
    score2++;
    start1 = true;
    return false;
  }
  return true;
}

void redirect(int dirY, int dirX, int y) {
  // increase x velocity if paddle strikes the ball
  if (dirX == 1) {
    ball.velx += ball.velx < 0 ? -1 : 1;
  } else { // otherwise slow it down
    if (ball.velx > BALL_SPEED || ball.velx < -BALL_SPEED) {
      ball.velx += ball.velx < 0 ? 1 : -1;
    }
  }
  // check for exceeding max velocity
  if (ball.velx > MAX_VELX) {
    ball.velx = MAX_VELX;
  } else if (ball.velx < -MAX_VELX) {
    ball.velx = -MAX_VELX;
  }

  // accelerate based on where it hits the paddle
  if (ball.y + BALL_SIZE / 2 <= y + PADDLE_REGION) {
    ball.vely -= 1;
  } else if (ball.y + BALL_SIZE / 2 >= y + PADDLE_HEIGHT - PADDLE_REGION) {
    ball.vely += 1;
  } else {
    int rand = randint(0, 10);
    if (rand < REDIRECT_CHANCE) {
      rand = randint(0, 1);
      ball.vely += rand ? 1 : -1;
    }    
  }
  
  // accelerate based on the motion of the paddle
  if (dirY == 1) {
    ball.vely += 1;
  } else if (dirY == -1) {
    ball.vely -= 1;
  }
  if (ball.vely >= MAX_VELY) {
    ball.vely = MAX_VELY;
  } else if (ball.vely <= -MAX_VELY) {
    ball.vely = -MAX_VELY;
  }
}

void predict(void) {
  if (ball.velx >= 0) {
    return;
  }
  // the time it will take for the ball to be when the paddle starts moving horizontally to strike the ball
  int time = (ball.x - paddle2.x - PADDLE_WIDTH - AI_MOVEDIST_X) / -ball.velx; // t = d / v

  // time it will take for the paddle to meet the ball after it starts moving to strike it
  time += (AI_MOVEDIST_X / (-ball.velx + PADDLE_SPEED_X));

  // how much the ball moves vertically in that time
  predictedY = ball.y + ball.vely * time;

  // simulate bouncing on the top or bottom
  while (predictedY < 0 || predictedY + BALL_SIZE > 159) {
    if (predictedY < 0) {
      predictedY = -predictedY;
    } else {
      predictedY = (predictedY < 160 ? 318 : 320) - predictedY - 2 * BALL_SIZE;
      predictedY = -predictedY;
    }
  }
  
  // apply inaccuracy
  int accuracy = 0;
  
  // increase inaccuracy based on the speed of the ball
  if (ball.vely < 0) {
    accuracy += (-ball.vely) * AI_ACCURACY;
  } else {
    accuracy += ball.vely * AI_ACCURACY;
  }
  // default x speed should give no inaccuracy
  if (ball.velx < 0) {
    accuracy += (-ball.velx - 1) * AI_ACCURACY;
  } else {
    accuracy += (ball.velx - 1) * AI_ACCURACY;
  }

  predictedY += randint(0, accuracy) - accuracy / 2 + BALL_SIZE / 2;
  
  moveTime = 119 - randint(AI_MOVETIME_MIN, AI_MOVETIME);
}