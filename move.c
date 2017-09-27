#include "move.h"

void move_up(Player player)
{
    if(player.next != NULL) {
        player.y < NUM_ROWS ? player.y++ : 0;
    } else {
        player.y > 0 ? player.y-- : 0;
    }
}

void move_down(Player player)
{
    //Check if player1 or player2
    if(player.next != NULL) {
        player.y > 0 ? player1.y--:0;
    } else {
        player.y < NUM_ROWS ? player.y++:0;
    }
}

void move_left(Player player)
{
    //Check player is not at left edge of led matrix
    player.x > 0 ? player.x--:0;
}

void move_right(Player player)
