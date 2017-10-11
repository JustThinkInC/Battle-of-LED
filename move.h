/* Description: Header files of player movement functions.
 * Author: George Khella
 * Date: September 2017
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

void move_up(Player* player, uint8_t y_change);

void move_down(Player* player, uint8_t y_change);

void move_left(Player* player);

void move_right(Player* player);
#endif
