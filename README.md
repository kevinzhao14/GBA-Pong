# GBA-Pong
Pong made for a GBA emulator in C

#Instructions
```
      ___         ___           ___           ___     
     /  /\       /  /\         /__/\         /  /\    
    /  /::\     /  /::\        \  \:\       /  /:/_   
   /  /:/\:\   /  /:/\:\        \  \:\     /  /:/ /\  
  /  /:/~/:/  /  /:/  \:\   _____\__\:\   /  /:/_/::\ 
 /__/:/ /:/  /__/:/ \__\:\ /__/::::::::\ /__/:/__\/\:\
 \  \:\/:/   \  \:\ /  /:/ \  \:\~~\~~\/ \  \:\ /~~/:/
  \  \::/     \  \:\  /:/   \  \:\  ~~~   \  \:\  /:/ 
   \  \:\      \  \:\/:/     \  \:\        \  \:\/:/  
    \  \:\      \  \::/       \  \:\        \  \::/   
     \__\/       \__\/         \__\/         \__\/    


This project is an implementation of the classic arcade game Pong, with a twist.



   ____        _      _        _             _      _____       _     _      
  / __ \      (_)    | |      | |           | |    / ____|     (_)   | |     
 | |  | |_   _ _  ___| | _____| |_ __ _ _ __| |_  | |  __ _   _ _  __| | ___ 
 | |  | | | | | |/ __| |/ / __| __/ _` | '__| __| | | |_ | | | | |/ _` |/ _ \
 | |__| | |_| | | (__|   <\__ \ || (_| | |  | |_  | |__| | |_| | | (_| |  __/
  \___\_\\__,_|_|\___|_|\_\___/\__\__,_|_|   \__|  \_____|\__,_|_|\__,_|\___|
                                                                             
                                                                             
There are two game modes: singleplayer and two-player. To select a game mode, use
the arrow keys to navigate to your choice, then press [START] (enter) to play.

In singleplayer, you play against the computer. There are four difficulties for the
computer: Easy, Normal, Hard, and Impossible. To select a difficulty, use the arrow
keys to navigate to your choice, then press [START] (enter) to play. To return to
the main menu, press [SELECT] (backspace).


   _____                            _             
  / ____|                          | |            
 | |  __  __ _ _ __ ___   ___ _ __ | | __ _ _   _ 
 | | |_ |/ _` | '_ ` _ \ / _ \ '_ \| |/ _` | | | |
 | |__| | (_| | | | | | |  __/ |_) | | (_| | |_| |
  \_____|\__,_|_| |_| |_|\___| .__/|_|\__,_|\__, |
                             | |             __/ |
                             |_|            |___/ 

Player 1 is on the right, and moves their paddle using the arrow keys.
Player 2 (or the computer) is on the left, and moves vertically using A and B, and
horizontally using Left and Right paddle.

The ball spawns in the middle of the screen in each round, and is served toward the
player who lost the previous round, or player 1 at the start. To hit the ball, simply
let the ball bounce off of your paddle.

If the ball bounces on the top of the paddle, it accelerates upward. If the ball
bounces on the bottom of the paddle, it accelerates downward. If the ball bounces
in the middle of the paddle, it has a 50% chance of not changing.

If the paddle is moving upward on contact, the ball accelerates downward. If the
paddle is moving downward on contact, the ball accelerates upward. These effects
stack, so if the ball hits the top of the paddle while the paddle is moving upward,
then the ball does not accelerate.

To score, the ball must hit the goal, or back bounds of the player's side. In other
words, if the ball hits the left-side of the game area, then player 1 scores a point.
There is no winning score, meaning you may play for as long as you would like.

To pause the game, press [START] (enter).
To exit the game and return to the menu, press [SELECT] (backspace).


   _____ _      _    _ ______ _ 
  / ____| |    | |  | |  ____| |
 | |  __| |    | |__| | |__  | |
 | | |_ | |    |  __  |  __| | |
 | |__| | |____| |  | | |    |_|
  \_____|______|_|  |_|_|    (_)
                                
                                
(Have a crack at the impossible bot :D)```