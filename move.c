/** @file   move.c
    @author George Khella
    @date   October 2017.
    @brief  Player movement.
*/

#include "move.h"
#include "stdlib.h"

//Movement functions look like the should be modular (imo).
//Refer to the image if confused about how the coord system works here
void move_up(Player* player, uint8_t y_change)
{
    if (player->pos.y - y_change >= 0) {
        player->pos.y -= y_change;
    }
}

void move_down(Player* player, uint8_t y_change)
{
    if (y_change < 10) {
        if (player->pos.y + y_change < 7 ) {
            player->pos.y += y_change;
        }
    } else {
        //Check if player1 or player2
        if(player->next != NULL) {
            if (player->pos.y < 6 ) {
                player->pos.y++;
            }
            //Need to fix this one, supposed to  be for player2
        } else {
            if (player->pos.y < LEDMAT_COLS_NUM) {
                player->pos.y++;
            }
        }
    }
}

void move_left(Player* player)
{
    //Check player is not at left boundary already
    if (player->pos.x > 0) {
        player->pos.x--;
    }
}

void move_right(Player* player)
{
    //Check player is not at right boundary
    if (player->pos.x < LEDMAT_COLS_NUM-1) {
        player->pos.x++;
    }
}
