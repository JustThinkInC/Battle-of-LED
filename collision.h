/* Description: Header files of collision detector.
 * Author: George Khella
 * Date: September 2017
 */
 
#ifndef COLLISION_H
#define COLLISION_H

//Check for collision between player and sandbags.
static int sandbag_collision (Player* player, uint8_t move_type)

//This function checks for collision between player and opponent
static int player_collision_check (Player* player)

//Check for collisions between bullet other game objects
static int bullet_collision(Bullet bullet)
#endif
