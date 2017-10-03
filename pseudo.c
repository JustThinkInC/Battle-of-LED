//TODO: add appropriate header files
#include "system.h"
#include "stdlib.h"
#include "pio.h"
//#include "button.h"
//#include "tinygl.h"
//#include "../fonts/font3x5_1.h"
#include "navswitch.h"

//My (George Khella) header files
#include "struct_init.h"
#include "hashmap.h"
#include "move.h"
#include "collision.h"



/* ISSUES/CONCERNS:
*/

/** RESOLVED ISSUES:
 * Both trenches captured --> Defeat
 * Previous player pointer --> doubly-linked list
 * Player selection --> Attacker, Defender
 * Players at same coord --> Christmas Truce
 **/
 
/** COMPLETED TASKS:
 * Player collision check
 * Sandbag collision check w/ hashmap
 * Trench/sandbag initialisation
 * Movement
 * Shooting
 **/

/** IDEAS: Maybe have LED1 (blue led) flash as soon as opponent in no
 *        man's land, with the flash rate proportional to distance from
 *        your trench?
 *        Easter egg? (Christmas Truce is one, maybe more if have time)
 *        Selection by year and/or country? higher year->tougher defence->slightly improved DPS
 *           -->CURRENT: Selection by Attacker/Defender maybe both
**/

/* TODO:
 * Game function for task scheduler
 * IR recieve&send
 * Check wich board is P1
 * Christmas truce
 * Game over
 * UI Selection
 * Sound
 */
 
//NOTE: SINCE BOARD IS ROTATED COLS IS ROWS AND ROWS IS COLS

//Polling rate of tasks in Hz
//Values may need adjustmenst after testing.
#define DISPLAY_TASK_RATE 250
#define GAME_TASK_RATE 100

//Movement types and values
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

//14 target sandbags with 2 health = 28 bullets + 28 for enemy = 56 bullets
#define MAX_NUM_BULLETS 56 

Bullet bulletPool[MAX_NUM_BULLETS];  //The bullet pool
uint8_t firstFree = 0;


void end_game(void) {
    //TODO: End game
    
}

Bullet* create_bullet(uint8_t x_pos, uint8_t y_pos, Player* target)
{
    Bullet* bullet = NULL;
    
    if (firstFree < MAX_NUM_BULLETS) {
        bullet = &bulletPool[firstFree];
        firstFree++;
       bullet->x = x_pos;
       bullet->y = y_pos;
       bullet->target = target;
    } 
    
    
    return bullet;
}


void damage_sandbag(SandBag sandbag)
{
    sandbag.health--;
    //sandbag->health == 0 ? turn off sandbag : lower brightness of that sandbag;
}


//Need to shorten this function...a lot!
//Handles shooting event
static void shoot (Player* player)
{
    Bullet* bullet;
    //If player1 fired the shot
    if(player->next != NULL) {
        bullet = create_bullet(player->x, player->y+1, player->next);
    } else {
        bullet = create_bullet(player->x, player->y+1, player->prev);
    }
    Player* target = bullet->target;
   while(bullet) {
        //If player1 then move bullet up else down (from p1 perspective!)
        player->next != NULL ? bullet->y++ : bullet->y--;
        SandBag sandbag = hash_contains(bullet->x, bullet->y);
        
        if (sandbag.health > 0 && sandbag.parent == target) {
            damage_sandbag(sandbag);
            //Delete bulllet?!
        } else if (bullet->x == target->x && bullet->y == target->y) {
            //delete bullet
            //end_game();
        }
        else {
            //(bullet->y >= LEDMAT_ROWS_NUM || bullet->y <= 0) ? delete(bullet) : 0;
        }
    }
}



//Initialise ledmatrix
/*
void led_init(void)
{
    //NOT PSUEDO, proper functioning code
    uint8_t i = 0;
    while (i <= LEDMAT_ROWS_NUM - 1) {
        pio_config_set(rows[i], PIO_OUTPUT_HIGH);
        i++;
    }
    i = 0;

    while (i <= LEDMAT_ROWS_NUM - 1) {
        pio_config_set(cols[i], PIO_OUTPUT_HIGH);
        i++;
    }
}
*/

//Initialise player positions and trenches
/*
static void init_positions (Player* player)
{
    while (player) {
        player->x = (uint8_t) (num_columns / 2); //Center of led matrix
        player->y = (player.next) ? 0 : LEDMAT_COLS_NUM*2;

        player = player->next;
    }

    player = player->prev;

    //TODO: Initialise trenches

    uint8_t i = 0;
    uint8_t x_pos = 0;
    uint8_t y_pos = 1;

    while (player) {
        while (i < TRENCH_DEPTH) {
            if(x_pos > LEDMAT_ROWS_NUM) {
                x_pos = 0;
                y_pos++;
            }
            player->sandbags[i].x = x_pos;
            player->sandbags[i].y = y_pos;
            player->sandbags[i].health = SANDBAG_HEALTH;
            player->sandbags[i].parent = player;
            hash_add(player->sandbags[i]);
            x_pos++;
        }
        player = player->next;
    }
}
*/


//Checks for game events (e.g. movement) and calls neccessary
//functions to handle them
void run_game(Player* player)
{
    uint8_t col_type = 0;

    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        col_type = sandbag_collision(player, UP);
        (col_type == 0) ? move_up(player) : 0;
    } else if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        col_type = sandbag_collision(player, DOWN);
        (col_type == 0) ? move_down(player) : 0;
    } else if (navswitch_push_event_p (NAVSWITCH_WEST)) {
        col_type = sandbag_collision(player, LEFT);
        (col_type == 0) ? move_left(player) : 0;
    } else if (navswitch_push_event_p (NAVSWITCH_EAST)) {
        col_type = sandbag_collision(player, RIGHT);
        (col_type == 0) ? move_right(player) : 0;
    } else if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
        shoot(player);
    }

    col_type = player_collision_check (player);
    col_type ? end_game() : 0;
}

int main(void)
{/*
    task_t tasks[] = {
        {.func = display_task, .period = TASK_RATE / DISPLAY_TASK_RATE},
        {.func = run_game, .period = TASK_RATE / GAME_TASK_RATE},
    };

    //TODO: Initialisation
    system_init();
    //init_positions (player);

    //TODO: TASK SCHEDULER
    task_schedule (tasks, ARRAY_SIZE (tasks));
    */
    return 0;
}
