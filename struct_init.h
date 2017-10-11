/* Description: Header file for the player, sandbag and bullet structs.
 * Author: George Khella
 * Date: October 2017
*/

#include "pio.h"
#include "tinygl.h"

#ifndef STRUCT_INIT_H
#define STRUCT_INIT_H
#define SANDBAG_HEALTH 2
#define SANDBAG_NUM LEDMAT_COLS_NUM 
typedef struct sandbag_s SandBag;
typedef struct bullet_s Bullet;
typedef struct player_s Player;

struct sandbag_s {
    uint8_t health;
    tinygl_point_t pos;
    Player* parent;
};

struct player_s {
    tinygl_point_t pos;
    Player* next;
    Player* prev;
    SandBag sandbags[SANDBAG_NUM];
};

struct bullet_s {
    tinygl_point_t pos;
    Player* target;
};
#endif
