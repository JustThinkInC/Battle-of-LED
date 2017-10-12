/* Description: Code for collision detector.
 * Author: George Khella
 * Date: September 2017
 */

#include "collision.h"
#include "struct_init.h"
#include "stdlib.h"

//This holds the row number of the trenches, 5 is for p1, 1 is for p1
static uint8_t def_line[2] = {5, 1};

//Check if player collided with a sandbag
//Return 0, no collision
//Return 1, collision with friendly sandbag, collision resolved if up/down
//Return 2, collision with enemy sandbag
uint8_t sandbag_collision (Player* player, uint8_t move_type)
{
    /*Create a dummy variable to hold the player's new positions if
     desire movemet were to occur. */
    uint8_t x_pos = player->pos.x;
    uint8_t y_pos = player->pos.y;

    switch (move_type) {
    case UP:
        y_pos--;
        break;
    case DOWN:
        y_pos++;
        break;
    case LEFT:
        x_pos--;
        x_pos < 0 ? x_pos = 0 : 0;
        break;
    case RIGHT:
        x_pos++;
    }

    if(y_pos != def_line[0] && y_pos != def_line[1]) {
        return 0;
    }

    //Collision detection O(1) way...
    SandBag sandbag_temp = hash_contains(x_pos, y_pos);
    SandBag sandbag = sandbag_temp.parent->sandbags[sandbag_temp.slot];
    if (sandbag.health <= 0) {
        return 0;
    } else if (sandbag.parent != player) {
        return 1;
    } else if(sandbag.parent == player) {
        if (move_type == UP) {
            move_up(player, 2);
            return 1;
        } else if (move_type == DOWN) {
            if(y_pos == def_line[0]) {
                move_down(player, 2);
                return 1;
            }
        }
    }


    return 1;
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
