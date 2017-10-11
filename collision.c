/* Description: Code for collision detector.
 * Author: George Khella
 * Date: September 2017
 */

#include "collision.h"
#include "struct_init.h"
#include "stdlib.h"


//Check if player collided with a sandbag
//Return 0, no collision
//Return 1, collision with friendly sandbag, collision resolved
//Return 2, collision with enemy sandbag
uint8_t sandbag_collision (Player* player, uint8_t move_type)
{
    /*Create a dummy variable to hold the player's new positions if
     desire movemet were to occur. */
    uint8_t x_pos = player->pos.x;
    uint8_t y_pos = player->pos.y;

    if(move_type == UP) {
        y_pos--;
    } else if (move_type == DOWN) {
        y_pos++;
    } else if (move_type == LEFT) {
        x_pos--;
    } else if(move_type == RIGHT) {
        x_pos++;
    }

    //Collision detection O(1) way...
    SandBag sandbag = hash_contains(x_pos, y_pos);
    if (sandbag.health == 0) {
        return 0;
    } else {
        if (sandbag.parent != player) {
            return 2;
        } else {
            if (move_type == UP) {
                //If sandbag above does exist
                sandbag = hash_contains(sandbag.pos.x, sandbag.pos.y-1);
                if (sandbag.health > 0 && sandbag.parent == player) {
                    /*move_up(player);
                    move_up(player);
                    move_up(player);
                    */
                    move_up(player, 3);
                    return 1;
                } else {
                    move_up(player, 2);
                    /*move_up(player);
                    move_up(player);
                    */
                    return 1;
                }
            } else if (move_type == DOWN) {
                if(player->pos.y == 3) {// || player->pos.y == LEDMAT_COLS_NUM - 2) {
                   /* move_down(player);
                    move_down(player);
                    move_down(player);*/
                    move_down(player, 3);
                    return 1;
                } else {/*
                    move_down(player);
                    move_down(player);
                    move_down(player);
                    */
                    move_down(player, 2);
                    return 1;
                }
            } else if (move_type == LEFT) {
                if (player->sandbags[sandbag.pos.x-1].health == 0) {
                    if (player->sandbags[sandbag.pos.x-1].pos.y == y_pos) {
                        move_left (player);
                    }
                }
            } else if (move_type == RIGHT) {
                if (player->sandbags[sandbag.pos.x+1].health == 0) {
                    if (player->sandbags[sandbag.pos.x+1].pos.y == y_pos) {
                        move_right (player);
                    }
                }
            }
        }
    }

    return 0;
}

//This function checks for collision between player and opponent
uint8_t player_collision_check (Player* player)
{
    uint8_t x_pos = player->pos.x;
    uint8_t y_pos = player->pos.y;

    //Check if players on same coord (collision)
    if(player->next != NULL) {
        if (x_pos == player->next->pos.x && y_pos == player->next->pos.y) {
            return 1;
        }
    } else {
        if (x_pos == player->prev->pos.x && y_pos == player->prev->pos.y) {
            return 1;
        }
    }

    return 0;
}
