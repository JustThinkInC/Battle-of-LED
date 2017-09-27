#include "collision.h"

//This function checks for collision between player and opponent
static int player_collision_check (Player player)
{
    uint8_t x_pos = player.x;
    uint8_t y_pos = player.y;
    
    //Check if players on same coord (collision)
    if(player.next != NULL) {
        if (x_pos == player.next.x && y_pos == player.next.y) {
            return 1;
        }
    } else {
        if (x_pos == player.prev.x && y_pos == player.prev.y) {
            return 1;
        }
    }
    
    return 0;
}

//Check for collisions between bullet other game objects
static int bullet_collision(Bullet bullet)
{
    uint8_t sandbag_number = 0;
    SandBags sandbags = bullet.target.sandbags;
    
    if(bullet.target.x == bullet.x && bullet.target.y == bullet.y) {
        return 1;
    } else {
        while(sandbag_number <= sandbags) {
            
            if(sandbags[sandbag_number].x == bullet.x &&
                    sandbags[sandbag_number].y == bullet.y) {
                damage_sandbag(sandbags[sandbag_number]);
                delete(bullet);
                
                return 1;
            }
            
            sandbag_number++;
        }
    }
    
    
    return 0;
}
