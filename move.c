/* Description: Code for player movement functions.
 * Author: George Khella
 * Date: September 2017
 */

#include "move.h"
#include "stdlib.h"

//Movement functions look like the should be modular (imo).
//Refer to the image if confused about how the coord system works here
void move_up(Player* player)
{
    if(player->next != NULL) {
       if (player->pos.y < LEDMAT_COLS_NUM) { 
           player->old = player->pos;
           player->pos.y++;
      }
    } else {
       if (player->pos.y > 0) {
           player->old = player->pos;
           player->pos.y--;
        }
    }
}

void move_down(Player* player)
{
    //Check if player1 or player2
    if(player->next != NULL) {
       if (player->pos.y > 0) {
           player->old = player->pos;
           player->pos.y--;
        }
    } else {
        if (player->pos.y < LEDMAT_COLS_NUM) { 
           player->old = player->pos;
           player->pos.y++;
      }
    }
}

void move_left(Player* player)
{
    //Check player is not at left edge of led matrix
    if (player->pos.x > 0) {
        player->old = player->pos;
        player->pos.x--;
    }
}

void move_right(Player* player)
{
    //Check player is not at right edge of led matrix
    if (player->pos.x < LEDMAT_ROWS_NUM) {
        player->old = player->pos;
        player->pos.x++;
    }
}
