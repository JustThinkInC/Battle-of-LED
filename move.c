/** @file   move.c
    @author George Khella
    @date   October 2017.
    @brief  Player movement.
*/

#include "move.h"
#include "stdlib.h"

//This function moves the player up
void move_up(Player* player, uint8_t y_change)
{
    //Check player wont go off screen
    if (player->pos.y - y_change >= 0) {
        player->pos.y -= y_change;
    }
}

//This function moves the player down
void move_down(Player* player, uint8_t y_change)
{
    //Check player won't be going off screen
    if(player->pos.y + y_change <= LEDMAT_ROWS_NUM-1) {
        player->pos.y += y_change;
    }
}

//This function moves the player left by 1 unit
void move_left(Player* player)
{
    //Check player is not at left boundary already
    if (player->pos.x > 0) {
        player->pos.x--;
    }
}

//This function moves the player right by 1 unit
void move_right(Player* player)
{
    //Check player is not at right boundary
    if (player->pos.x < LEDMAT_COLS_NUM-1) {
        player->pos.x++;
    }
}
