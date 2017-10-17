/** @file   collision.h
    @author George Khella
    @date   October 2017.
    @brief  Collision detection header files.
*/

/* Description: Header files of collision detector.
 * Author: George Khella
 * Date: September 2017
 */
 
#include "struct_init.h"
#include "move.h"
#include "hashmap.h"

#ifndef COLLISION_H
#define COLLISION_H

//Check for collision between player and sandbags.
uint8_t sandbag_collision(Player* player, uint8_t move_type);

//This function checks for collision between players
uint8_t player_collision_check(Player* player);
#endif
