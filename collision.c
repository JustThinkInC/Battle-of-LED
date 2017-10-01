/* Description: Code for collision detector.
 * Author: George Khella
 * Date: September 2017
 */

#include "collision.h"

//Check if player collided with a sandbag
//Return 0, no collision
//Return 1, collision with friendly sandbag, collision resolved
//Return 2, collision with enemy sandbag
static int sandbag_collision (Player* player, uint8_t move_type)
{
    SandBag us = player.sandbags;
    (SandBag them = ) player.next != NULL ? player.next : player.prev;

    uint8_t sandbag = 0;
    uint8_t x_pos = player.x;
    uint8_t y_pos = player.y;

    if(move_type == UP) {
        y_pos++;
    } else if (move_type == DOWN) {
        y_pos--;
    } else if (move_type == LEFT) {
        x_pos--;
    } else if(move_type == RIGHT) {
        x_pos++;
    }

    //Collision detection O(1) way...
    SandBag sandbag = hash_contains(player);
    if (!sandbag) {
        return 0;
    } else if (sandbag) {
        if (sandbag.parent != player) {
            return 2;
        } else {
            if (move_type == UP) {
                //If sandbag above doesn't exist
                if !us[sandbag.x].y{
                    move_up();
                    move_up();

                    return 1;
                } else {
                    move_up();
                    move_up();
                    move_up();

                    return 1;
                }
            } else if (move_type == DOWN) {
                if(player.y == 2 || player.y == NUM_ROWS - 2) {
                    move_down();
                    move_down();

                    return 1;
                } else {
                    move_down();
                    move_down();
                    move_down();

                    return 1;
                }
            } else if (move_type == LEFT) {
                if (player[sandbag.x-1]) {
                    if (player[sandbag.x].y == y_pos) {
                        move_left;
                    }
                }
            } else if (move_type == RIGHT) {
                if (player[sandbag.x+1]) {
                    if (player[sandbag.x].y == y_pos) {
                        move_right ();
                    }
                }
            }
        }
    }
    
    
    return 0;
}

//This function checks for collision between player and opponent
static int player_collision_check (Player* player)
{
    uint8_t x_pos = player.x;
    uint8_t y_pos = player.y;

    //Check if players on same coord (collision)
    if(player.next != NULL) {
        if (x_pos == player.next.x && y_pos == player.next.y) {
            christmas_truce();
            return 1;
        }
    } else {
        if (x_pos == player.prev.x && y_pos == player.prev.y) {
            christmas_truce();
        }
    }

    return 0;
}

//Check for collisions between bullet other game objects
static int bullet_collision(Bullet bullet)
{
    SandBag sandbags = bullet.target.sandbags;
    Player target = bullet.target;
    while (bullet) {
        SandBag sandbag = hash_contain(bullet);
        if (sandbag) {
            if (sandbag.parent == bullet.target) {
                damage_sandbag(sandbag);
                bullet = NULL; //Delete bulllet?!
                
                return 1;
            }
        } else if (bullet.x == target.x && bullet.y == target.y) {
            bullet = NULL;
            end_game();
        }
    }


    return 0;
}
