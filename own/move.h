/** @file   move.h
    @author George Khella
    @date   October 2017.
    @brief  Player movement header files.
*/

#include "pio.h"
#include "struct_init.h"

#ifndef MOVE_H
#define MOVE_H

//Movement types and values
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

//Move player up certain distance
void move_up(Player* player, uint8_t y_change);

//Move player down certain distance
void move_down(Player* player, uint8_t y_change);

//Move player 1 unit left
void move_left(Player* player);

//Move player 1 unit right
void move_right(Player* player);
#endif
