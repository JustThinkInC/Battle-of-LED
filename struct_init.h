/* Description: Header file for the player, sandbag and bullet structs.
 * Author: George Khella
 * Date: October 2017
*/

#include "pio.h"

#ifndef STRUCT_INIT_H
#define STRUCT_INIT_H
#define SANDBAG_HEALTH 2
typedef struct sandbag_s SandBag;
typedef struct bullet_s Bullet;
typedef struct player_s Player;

struct sandbag_s {
    uint8_t health;
    uint8_t x;
    uint8_t y;
    Player* parent;
};

struct player_s {
    uint8_t x;
    uint8_t y;
    Player* next;
    Player* prev;
    //Rows since we have board rotated
    SandBag sandbags[LEDMAT_ROWS_NUM * 2];
};

struct bullet_s {
    uint8_t x;
    uint8_t y;
    Player* target;
};


#endif
