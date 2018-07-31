# BATTLE OF LED, 1914
Made by George Khella & Theo Harbers

By continuing and/or downloading, you confirm that you have read and accepted 
the [license](LICENSE).

The game is based around World War I. Each player has a trench of one sandbag row.
The objective is to reach the other side of the board or kill the opponent. 
Destroy opponent's sandbag to create a hole to get through. 


# How to Run
For compiling and running on a Linux machine do the following steps:
1) Make sure you have the drivers/avr, drivers, utils, fonts and extras folders 
   and files. Re-clone/download the git if not.
2) To compile the game, this will produce .hex, .o, and .bin files, use the following command:
    > make
3) To compile and run the game on the avr:
    > make program
    
Sound is available with using a piezo buzzer.

# Playing the game:
1) Scrolling title text will appear, simply push down the joystick to start the 
   game.
2) Every player has a trench and 30 bullets (more than enough!), every sandbag
   has 2 hit points (HP), each bullet deals 1 damage HP. Every player has only
   1 HP. Objective is to capture opponent's trench or take them down.
3) Move by pushing the joystick in the desired direction.
4) Push to shoot
5) When game is over, scrolling text will notify you of the outcome, 
   push joystick to return to the menu.
6) There is an "Easter" egg, find it! Tis probably the best outcome (hint hint)
   Reading the code to find the Easter egg does not mean you've found it :)
   
