const WIDTH = 240;          // width of the game/canvas; must be even
const HEIGHT = 160;         // height of the game/canvas; must be even
const SCALE = 4;            // how much to scale the graphics
const FPS = 60;             // framerate of the game and calculations

const BALL_SIZE = 4;        // size of the ball (diameter)
const PADDLE_HEIGHT = 20;   // height of the paddles
const PADDLE_WIDTH = 2;     // width of the paddles
const PADDLE_REGION = 8;    // size of the top and bottom regions on the paddle for redirection
const PADDLE_OFFSET = 10;   // how far from the walls the paddles spawn
const PADDLE_SPEED_Y = 3;   // vertical speed of the paddles
const PADDLE_SPEED_X = 2;   // horizontal speed of the paddles
const BALL_SPEED = 2;       // base/min speed of the ball
const SCORE_OFFSET_Y = 10;  // the vertical offset of the score displays
const SCORE_OFFSET_X = 20;  // the horizontal offset of the score displays from the center 
const RESET_TIME = 60;      // how many frames in between each round
const REDIRECT_CHANCE = 5;  // chance out of = 10; for a middle hit to change directions
const MAX_VELY = 5;         // max vertical velocity of the ball
const MAX_VELX = 4;         // max horizontal velocity of the ball

var diff;
// base inaccuracy of the ai 
var AI_ACCURACY;
// minimum distance from the center the ai can move at
// negative means on the side of player 1
var AI_MOVETIME_MIN;
// maximum distance from the center the ai can move at
var AI_MOVETIME;
// distance the ball is from the paddle before the ai starts moving right to hit it
var AI_MOVEDIST_X;
// distance the ball must be before the ai resets horizontally
var AI_RESET_X;

// image data
const menuSingle = new Image();
menuSingle.src = "images/menuSingle.png";
const menuTwo = new Image();
menuTwo.src = "images/menuTwo.png";
const diffEasy = new Image();
diffEasy.src = "images/diffEasy.png";
const diffNormal = new Image();
diffNormal.src = "images/diffNormal.png";
const diffHard = new Image();
diffHard.src = "images/diffHard.png";
const diffImpossible = new Image();
diffImpossible.src = "images/diffImpossible.png";

// 1 = easy   2 = normal    3 = hard    4 = impossible
function setDiff(d) {
    diff = d;
    AI_ACCURACY = diff == 4 ? 0 : ((4 - diff) * 4 + 2);
    AI_MOVETIME_MIN = diff == 4 ? -120 : ((diff - 1) * -20);
    AI_MOVETIME = diff == 4 ? -120 : ((3 - diff) * 20);
    AI_MOVEDIST_X = diff > 2 ? 50 : -100;
    AI_RESET_X = diff > 2 ? 60 : -100;
}
setDiff(0);

const ball = {
    x : 0,
    y : 0,
    velX : 0,
    velY : 0
}

// right paddle
const paddle1 = {
    x : 0,
    y : 0
}

// left paddle
const paddle2 = {
    x : 0,
    y : 0
}