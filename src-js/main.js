var state = "START";    // state of the game

var score1 = 0;         // score of player 1 (right-side)
var score2 = 0;         // score of player 2 (left-side)

var frameCounter = 0;
var startFrame = 0;     // the # of the total frame for the game to start at
var start1 = true;      // which side to serve to

var predictedY = 0;     // the predicted Y position on contact by the AI
var moveTime = 0;       // when the AI will start moving to hit the ball

// 0 = none   1 = singleplayer    2 = multiplayer
var gamemode = 0;

// Save current and previous state of button input.
var keys = [];
var previousKeys = [];
var currentKeys = [];

var timer = null;

function main() {
    // Load the current state of the buttons
    currentKeys = [];
    for (let i = 0; i < keys.length; i++) {
        currentKeys.push(keys[i]);
    }
    
    switch(state) {
        case "START":
            // load menu screen
            state = "MENU";
            gamemode = 1;
            drawFullscreenImage(menuSingle);
            break;
        case "MENU":
            // if pressing start, go to difficulty screen for singleplayer or play screen for two player
            if (keyPressed(START)) {
                if (gamemode == 1) {
                    drawFullscreenImage(diffEasy);
                    setDiff(1);
                    state = "DIFF";
                } else {
                    state = "LOAD";
                }
                break;
            }
            
            // selecting game modes using arrow keys
            if (gamemode == 1 && keyDown(P1_DOWN)) {
                gamemode = 2;
                drawFullscreenImage(menuTwo);
            } else if (gamemode == 2 && keyDown(P1_UP)) {
                gamemode = 1;
                drawFullscreenImage(menuSingle);
            }
            break;
        case "DIFF":
            // start game
            if (keyPressed(START)) {
                state = "LOAD";
                break;
            } else if (keyPressed(SELECT)) {
                gamemode = 1;
                drawFullscreenImage(menuSingle);
                state = "MENU";
                break;
            }
            
            // selecting difficulty using arrow keys
            let sw = false;
            if (diff < 4 && keyPressed(P1_DOWN)) {
                setDiff(diff + 1);
                sw = true;
            } else if (diff > 1 && keyPressed(P1_UP)) {
                setDiff(diff - 1);
                sw = true;
            }
            
            if (sw) {
                switch(diff) {
                    case 1:
                        drawFullscreenImage(diffEasy);
                        break;
                    case 2:
                        drawFullscreenImage(diffNormal);
                        break;
                    case 3:
                        drawFullscreenImage(diffHard);
                        break;
                    case 4:
                        drawFullscreenImage(diffImpossible);
                        break;
                    default:
                        break;
                }
            }
            break;
        case "LOAD":
            // reset paddle positions
            paddle1.x = WIDTH - PADDLE_OFFSET - PADDLE_WIDTH;
            paddle1.y = (HEIGHT - PADDLE_HEIGHT) / 2;
            paddle2.x = PADDLE_OFFSET;
            paddle2.y = (HEIGHT - PADDLE_HEIGHT) / 2;
    
            // reset scores
            score2 = 0;
            score1 = 0;
    
            // reset serve side
            start1 = true;
    
            // draw play screen background  
            drawFullscreenRect(BG_COLOR);
            state = "RESET";
            break;
        case "RESET":
            // reset ball position
            ball.x = (WIDTH - BALL_SIZE) / 2;
            ball.y = (HEIGHT - BALL_SIZE) / 2;
            ball.velx = 0;
            ball.vely = 0;
        
            // calculate when to start the round
            startFrame = frameCounter + RESET_TIME;
        
            // reset AI vars
            moveTime = WIDTH / 2 - 1;
            predictedY = HEIGHT / 2 - 1;
        
            // start            
            state = "PLAY";
            break;
        case "PLAY":
            // reset game
            if (keyDown(SELECT)) {
                state = "START";
                break;
            }
            
            // pause game
            if (keyPressed(START)) {
                drawString(WIDTH / 2 - 1 - (3 * FONT_WIDTH), HEIGHT / 2 - 1 - (2 * FONT_HEIGHT), "PAUSED", FONT_COLOR);
                drawString(WIDTH / 2 - 1 - (11 * FONT_WIDTH), HEIGHT / 2 + FONT_HEIGHT, "Press START to resume.", FONT_COLOR);
                state = "PAUSE";
                break;
            }
            
            // start the round
            if (ball.velx == 0 && frameCounter >= startFrame) {
                // generate ball velocity
                ball.velx = (start1 ? 1 : -1) * BALL_SPEED;
                ball.vely = 0;
                
                // predict for the AI if serving to it
                if (!start1) {
                    predict();
                }
                
            }
            // run the game cycle
            run();
            break;
        case "PAUSE":
            // decrement frame counter so stuff dependent on the frame # don't break
            frameCounter--;
            
            // resume game
            if (keyPressed(START)) {
                drawFullscreenRect(BG_COLOR);
                state = "PLAY";
                break;
            }
            
            // reset game
            if (keyDown(SELECT)) {
                state = "START";
            }
            break;
        default:
            break;
    }
    previousKeys = [];
    for (let i = 0; i < currentKeys.length; i++) {
        previousKeys.push(currentKeys[i]);
    }
    frameCounter++;
}

function run() {
    // save previous ball positions to erase with
    let ballx = ball.x;
    let bally = ball.y;
    
    let paddle1x = paddle1.x;
    let paddle1y = paddle1.y;
    let paddle2x = paddle2.x;
    let paddle2y = paddle2.y;
    
    
    // calculate stuff
    let calc = calculate();
    
    // erase old ball
    drawRect(ballx, bally, BALL_SIZE, BALL_SIZE, BG_COLOR);
    
    // reset if someone loses, otherwise, draw the new ball position
    if (!calc) {
        state = "RESET";
    } else {
        drawRect(ball.x, ball.y, BALL_SIZE, BALL_SIZE, BALL_COLOR);
    }
    
    // erase old paddle vertical pixels
    if (paddle1.y > paddle1y) {
        drawRect(paddle1x, paddle1y, PADDLE_WIDTH, paddle1.y - paddle1y, BG_COLOR);
    } else if (paddle1.y < paddle1y) {
        drawRect(paddle1x, paddle1.y + PADDLE_HEIGHT, PADDLE_WIDTH, paddle1y - paddle1.y, BG_COLOR);
    }
    if (paddle2.y > paddle2y) {
        drawRect(paddle2x, paddle2y, PADDLE_WIDTH, paddle2.y - paddle2y, BG_COLOR);
    } else if (paddle2.y < paddle2y) {
        drawRect(paddle2x, paddle2.y + PADDLE_HEIGHT, PADDLE_WIDTH, paddle2y - paddle2.y, BG_COLOR);
    }
    
    // erase old paddle horizontal pixels
    if (paddle1.x > paddle1x) {
        drawRect(paddle1x, paddle1y, paddle1.x - paddle1x, PADDLE_HEIGHT, BG_COLOR);
    } else if (paddle1.x < paddle1x) {
        drawRect(paddle1.x + PADDLE_WIDTH, paddle1y, paddle1x - paddle1.x, PADDLE_HEIGHT, BG_COLOR);
    }
    if (paddle2.x > paddle2x) {
        drawRect(paddle2x, paddle2y, paddle2.x - paddle2x, PADDLE_HEIGHT, BG_COLOR);
    } else if (paddle2.x < paddle2x) {
        drawRect(paddle2.x + PADDLE_WIDTH, paddle2y, paddle2x - paddle2.x, PADDLE_HEIGHT, BG_COLOR);
    }
    
    // draw paddles
    drawRect(paddle1.x, paddle1.y, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_COLOR);
    drawRect(paddle2.x, paddle2.y, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_COLOR);
    
    // draw scores
    drawScore(score1, true, FONT_COLOR);
    drawScore(score2, false, FONT_COLOR);    
}

function drawScore(score, a, color) {
    if (a) {
        drawString(WIDTH / 2 + (score + "").length * FONT_WIDTH + SCORE_OFFSET_X, SCORE_OFFSET_Y, score, color);
    } else {
        drawString(WIDTH / 2 - 1 - (score + "").length * FONT_WIDTH - SCORE_OFFSET_X, SCORE_OFFSET_Y, score, color);
    }
}

function eraseScore(score, a) {
    let width = (score + "").length * FONT_WIDTH
    if (a) {
        drawRect(WIDTH / 2 + width + SCORE_OFFSET_X - 2, SCORE_OFFSET_Y - 2, width + 4, FONT_HEIGHT + 4, BG_COLOR);
    } else {
        drawRect(WIDTH / 2 - 1 - width - SCORE_OFFSET_X - 2, SCORE_OFFSET_Y - 2, width + 4, FONT_HEIGHT + 4, BG_COLOR);
    }
}

function calculate() {
    // whether the paddle has moved up/down and left/right
    let moveAy = 0;
    let moveAx = 0;
    
    if (keyDown(P1_UP)) {
        paddle1.y -= PADDLE_SPEED_Y;
        moveAy = 1;
    }
    if (keyDown(P1_DOWN)) {
        paddle1.y += PADDLE_SPEED_Y;
        moveAy = -1;
    }
    if (keyDown(P1_RIGHT)) {
        paddle1.x += PADDLE_SPEED_X;
        moveAx = -1;
    }
    if (keyDown(P1_LEFT)) {
        paddle1.x -= PADDLE_SPEED_X;
        moveAx = 1;
    }  
    
    // keep paddle in-bounds
    if (paddle1.y < 0) {
        paddle1.y = 0;
        moveAy = 0;
    } else if (paddle1.y + PADDLE_HEIGHT > HEIGHT) {
        paddle1.y = HEIGHT - PADDLE_HEIGHT;
        moveAy = 0;
    }
    if (paddle1.x + PADDLE_WIDTH > WIDTH) {
        paddle1.x = WIDTH - PADDLE_WIDTH;
        moveAx = 0;
    } else if (paddle1.x < WIDTH / 2 + BALL_SIZE) {
        paddle1.x = WIDTH / 2 + BALL_SIZE;
        moveAx = 0;
    }
    
    // same as ^ but for the left-paddle
    let moveBy = 0;
    let moveBx = 0;
    if (gamemode == 2) {  // if two-player, allow player to move paddle
        if (keyDown(P2_UP)) {
            paddle2.y -= PADDLE_SPEED_Y;
            moveBy = 1;
        }
        if (keyDown(P2_DOWN)) {
            paddle2.y += PADDLE_SPEED_Y;
            moveBy = -1;
        }
        if (keyDown(P2_LEFT)) {
            paddle2.x -= PADDLE_SPEED_X;
            moveBx = -1;
        }
        if (keyDown(P2_RIGHT)) {
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
    } else if (paddle2.y + PADDLE_HEIGHT > HEIGHT) {
        paddle2.y = HEIGHT - PADDLE_HEIGHT;
        moveBy = 0;
    }
    if (paddle2.x < 0) {
        paddle2.x = 0;
        moveBx = 0;
    } else if (paddle2.x + PADDLE_WIDTH > WIDTH / 2 - 1 - BALL_SIZE) {
        paddle2.x = WIDTH / 2 - 1 - BALL_SIZE - PADDLE_WIDTH;
        moveBx = 0;
    }
    
    // move the ball
    ball.x += ball.velx;
    ball.y += ball.vely;
    
    // border collisions
    if (ball.y < 0) {
        ball.y = -ball.y;
        ball.vely = - ball.vely;
    } else if (ball.y + BALL_SIZE > HEIGHT - 1) {
        ball.y = HEIGHT * 2 - (ball.y < HEIGHT ? 2 : 0) - ball.y - 2 * BALL_SIZE;
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
        // drawScore(score1, true, BG_COLOR);
        eraseScore(score1, true);
        score1++; // increment score
        start1 = false; // end round/restart
        return false;
    } else if (ball.x > WIDTH - 1) {
        // drawScore(score2, false, BG_COLOR);
        eraseScore(score2, false);
        score2++;
        start1 = true;
        return false;
    }
    return true;
}

function redirect(dirY, dirX, y) {
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
        let rand = randInt(0, 10);
        if (rand < REDIRECT_CHANCE) {
            rand = randInt(0, 1);
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

function predict() {
    if (ball.velx >= 0) {
        return;
    }
    // the time it will take for the ball to be when the paddle starts moving horizontally to strike the ball
    let time = (ball.x - paddle2.x - PADDLE_WIDTH - AI_MOVEDIST_X) / -ball.velx; // t = d / v
    
    // time it will take for the paddle to meet the ball after it starts moving to strike it
    time += (AI_MOVEDIST_X / (-ball.velx + PADDLE_SPEED_X));
    
    // how much the ball moves vertically in that time
    predictedY = ball.y + ball.vely * time;
    
    // simulate bouncing on the top or bottom
    while (predictedY < 0 || predictedY + BALL_SIZE > HEIGHT - 1) {
        if (predictedY < 0) {
            predictedY = -predictedY;
        } else {
            predictedY = 2 * HEIGHT - (predictedY < HEIGHT ? 2 : 0) - predictedY - 2 * BALL_SIZE;
            predictedY = -predictedY;
        }
    }
    
    // apply inaccuracy
    let accuracy = 0;
    
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
    
    predictedY += randInt(0, accuracy) - accuracy / 2 + BALL_SIZE / 2;
    
    moveTime = WIDTH / 2 - 1 - randInt(AI_MOVETIME_MIN, AI_MOVETIME);    
}

function randInt(min, max) {
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

function start() {
    if (timer == null) {
        timer = setInterval(main, 1000 / FPS);
    }
}

function stop() {
    if (timer != null) {
        clearInterval(timer);
        timer = null;
    }
}

menuSingle.addEventListener("load", start);

function keyDown(key) {
    return currentKeys.includes(key);
}

function keyPressed(key) {
    return keyDown(key) && !previousKeys.includes(key);
}

document.addEventListener("keydown", function(e) {
   let code = e.code;
   if (!keys.includes(code)) {
       keys.push(code);
   }
});

document.addEventListener("keyup", function(e) {
    let code = e.code;
    if (keys.includes(code)) {
        keys.splice(keys.indexOf(code), 1);
    }
})