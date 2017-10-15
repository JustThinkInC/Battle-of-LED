/** @file   struct_init.h
    @author George Khella
    @date   October 2017.
    @brief  Struct initialiser for player, sandbag and bullets.
*/

#include "pio.h"
#include "tinygl.h"

#ifndef STRUCT_INIT_H
#define STRUCT_INIT_H
#define SANDBAG_HEALTH 2
#define SANDBAG_NUM LEDMAT_COLS_NUM 
//5 target sandbags with 2 health = 10 bullets + 20 for enemy = 30 bullets
#define MAX_NUM_BULLETS 30

typedef struct sandbag_s SandBag;
typedef struct bullet_s Bullet;
typedef struct player_s Player;


struct sandbag_s {
    uint8_t health;
    tinygl_point_t pos;
    uint8_t slot;
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
    bool inactive;
    Player* target;
};
#endif
