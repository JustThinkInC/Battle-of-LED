/* Description: Code for player movement functions.
 * Author: George Khella
 * Date: September 2017
 */

#include "move.h"
#include "stdlib.h"

//Movement functions look like the should be modular (imo).
//Refer to the image if confused about how the coord system works here
void move_up(Player* player, uint8_t by_y)
{
    if (by_y < 10) {
        if (player->pos.y - by_y >= 0) {
            player->pos.y -= by_y;
        }
    } else {
        if(player->next != NULL) {
            if (player->pos.y > 0) {
                player->pos.y--;
            }
        } else {
            if (player->pos.y > 0) {
                player->pos.y--;
            }
        }
    }
}

void move_down(Player* player, uint8_t by_y)
{
    if (by_y < 10) {
        if (player->pos.y + by_y < 7 ) {
                player->pos.y += by_y;
            }
    } else {
        //Check if player1 or player2
        if(player->next != NULL) {
            if (player->pos.y < 6 ) {
                player->pos.y++;
            }
            //Need to fix this one
        } else {
            if (player->pos.y < LEDMAT_COLS_NUM) {
                player->pos.y++;
            }
        }
    }
}

void move_left(Player* player)
{
    //Check player is not at left edge of led matrix
    if (player->pos.x > 0) {
        player->pos.x--;
    }
}

void move_right(Player* player)
{
    //Check player is not at right edge of led matrix
    if (player->pos.x < LEDMAT_COLS_NUM-1) {
        player->pos.x++;
    }
}
