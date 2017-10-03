/* Description: Header files of collision detector.
 * Author: George Khella
 * Date: September 2017
 */
 
#include "struct_init.h"
#include "move.h"
#include "hashmap.h"


#ifndef COLLISION_H
#define COLLISION_H

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

//Check for collision between player and sandbags.
uint8_t sandbag_collision(Player* player, uint8_t move_type);

//This function checks for collision between player and opponent
uint8_t player_collision_check(Player* player);
#endif
