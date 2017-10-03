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
        player->y < LEDMAT_COLS_NUM ? player->y++ : 0;
    } else {
        player->y > 0 ? player->y-- : 0;
    }
}

void move_down(Player* player)
{
    //Check if player1 or player2
    if(player->next != NULL) {
        player->y > 0 ? player->y--:0;
    } else {
        player->y < LEDMAT_COLS_NUM ? player->y++:0;
    }
}

void move_left(Player* player)
{
    //Check player is not at left edge of led matrix
    player->x > 0 ? player->x--:0;
}

void move_right(Player* player)
{
    //Check player is not at right edge of led matrix
    player->x < LEDMAT_ROWS_NUM ? player->x++:0;
}
