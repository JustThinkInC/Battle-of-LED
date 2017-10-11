#include "system.h"
#include "stdlib.h"
#include "pio.h"
#include "tinygl.h"
#include "task.h"
#include "../fonts/font5x7_1.h"
#include "navswitch.h"
#include "led.h"

//My (George Khella) header files
#include "struct_init.h"
#include "hashmap.h"
#include "move.h"
#include "collision.h"


/* ISSUES/CONCERNS:
*/

/* TODO:
 * Game function for task scheduler
 * IR recieve&send
 * Check wich board is P1
 * Christmas truce
 * Game over
 * UI Selection
 * Sound
 */


//Polling rate of tasks in Hz
//Values may need adjustmenst after testing.
#define DISPLAY_TASK_RATE 250
#define GAME_TASK_RATE 100

//Movement types and values
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

//10 target sandbags with 2 health = 20 bullets + 10 for enemy = 30 bullets
#define MAX_NUM_BULLETS 30

Bullet bulletPool[MAX_NUM_BULLETS]; //The bullet pool
uint8_t firstFree = 0;

bool state = 0; //if bullet hits something
bool text = 0;
Player player1;
Player player2;


void display_msg (const char* msg)
{
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (10);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    tinygl_point_t pos;
    pos.x = 0;
    pos.y = 0;
    tinygl_draw_message(msg, pos, 1);
}

void draw(void)
{
    tinygl_clear();
    if (!text) {
        tinygl_draw_point (player1.pos, 1);
        tinygl_draw_point (player2.pos, 1);

        Player* player = &player1;

        while (player != NULL) {
            uint8_t i = 0;
            while(i < SANDBAG_NUM) {
                SandBag sandbag = player->sandbags[i];
                if (sandbag.health > 0) {
                    tinygl_draw_point (sandbag.pos, 1);
                }
                i++;
            }
            player = player->next;
            i = 0;
        }

        if (firstFree > 0 && &bulletPool[firstFree-1] != NULL) {
            Bullet bullet = bulletPool[firstFree-1];
            tinygl_draw_point(bullet.pos, 1);
            tinygl_update();
            timer_wait (160);
        }

    } else {
        display_msg("Victory");
    }

}

void end_game(Player* player)
{
    //TODO: End game
    if (player->next != NULL) {
        text = 1;
        draw();
        //   player1 message = "VICTORY";
        //  player2 message = "DEFEAT";

        //  tinygl_update();

    } else {
        //player1 message = "DEFEAT";
        //player2 message = "VICTORY";
    }
    /*
    wait like 5 seconds then bring back to main menu;
    maybe play a tune for each player depending on the outcome
    */

}

void christmas_truce(void)
{
    //TODO: End game

    // display message = "Christmas Truce, 1914";
    //play melody = "We wish you a Merry Christmas";
    //back to menu after melody ends;
}

Bullet* create_bullet(uint8_t x_pos, uint8_t y_pos, Player* target)
{
    Bullet* bullet = NULL;

    if (firstFree < MAX_NUM_BULLETS) {
        bullet = &bulletPool[firstFree];
        firstFree++;
        bullet->pos.x = x_pos;
        bullet->pos.y = y_pos;
        bullet->target = target;
    }


    return bullet;
}


void damage_sandbag(SandBag sandbag)
{
    sandbag.health--;
}


//Need to shorten this function...a lot!
//Handles shooting event
static void shoot (Player* player)
{
    Bullet* bullet;
    //If player1 fired the shot
    if(player->next != NULL) {
        bullet = create_bullet(player->pos.x, player->pos.y-1, player->next);
    } else {
        bullet = create_bullet(player->pos.x, player->pos.y+1, player->prev);
    }
    Player* target = bullet->target;
    while(bullet != NULL) {

        //If player1 then move bullet up else down (from p1 perspective!)
        player->next != NULL ? bullet->pos.y-- : bullet->pos.y--;
        SandBag sandbag = hash_contains(bullet->pos.x, bullet->pos.y);
        draw();
        if (sandbag.health > 0 && sandbag.parent == target) {
            damage_sandbag(sandbag);
            bullet = NULL;
        } else if (bullet->pos.x == target->pos.x && bullet->pos.y == target->pos.y) {
            bullet = NULL;
            state = !state;
            led_set (LED1, state);
            end_game(&player1);
        } else if (bullet->pos.y >= LEDMAT_ROWS_NUM || bullet->pos.y < 0) {
            bullet = NULL;
            state = !state;
            led_set (LED1, state);
        }
    }
}



static void display_task (__unused__ void *data)
{
    tinygl_update();
}

//Initialise player positions and trenches
void init_positions (Player* player)
{
    //Set next and prev to avoid
    player1.next = &player2;
    player2.prev = &player1;


    bool init = 1;
    while (init) {
        player->pos.x = ((LEDMAT_COLS_NUM-1) / 2); //Center of led matrix
        player->pos.y = (player->next != NULL) ? LEDMAT_ROWS_NUM-1 : LEDMAT_COLS_NUM*2;

        player = player->next;
        init = 0;
    }

    player = player->prev; //Back to player1

    uint8_t i = 0;
    uint8_t x_pos = 0;
    uint8_t y_pos = LEDMAT_ROWS_NUM - 2;

    //while (player != NULL) {

    while (i < SANDBAG_NUM) {
        if(x_pos > LEDMAT_COLS_NUM-1) {
            x_pos = 0;
            y_pos--;
        }
        player->sandbags[i].pos.x = x_pos;
        player->sandbags[i].pos.y = y_pos;
        player->sandbags[i].health = SANDBAG_HEALTH;
        player->sandbags[i].parent = player;
        hash_add(player->sandbags[i]);
        x_pos++;
        i++;
    }

    //player = player->next;
    i = 0;
    //}
    draw();
}


//Checks for game events (e.g. movement) and calls neccessary
//functions to handle them
//void run_game(Player* player)
static void run_game_task (__unused__ void *data)
{
    
    uint8_t col_type = 0;
    //If game over, we don't read input from navswitch
    !text ? navswitch_update() : 0;
    
    
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        col_type = sandbag_collision(&player1, UP);
        (col_type == 0) ? move_up(&player1, 10) : 0;
        draw();
    } else if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        col_type = sandbag_collision(&player1, DOWN);
        (col_type == 0) ? move_down(&player1, 10) : 0;
        draw();
    } else if (navswitch_push_event_p (NAVSWITCH_WEST)) {
        col_type = sandbag_collision(&player1, LEFT);
        (col_type == 0) ? move_left(&player1) : 0;
        draw();
    } else if (navswitch_push_event_p (NAVSWITCH_EAST)) {
        col_type = sandbag_collision(&player1, RIGHT);
        (col_type == 0) ? move_right(&player1) : 0;
        draw();
    } else if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
        shoot(&player1);
    }
        
    col_type = player_collision_check (&player1);
    col_type ? end_game(&player1) : 0;

}


int main(void)
{
    task_t tasks[] = {
        {.func = display_task, .period = TASK_RATE / DISPLAY_TASK_RATE},
        {.func = run_game_task, .period = TASK_RATE / GAME_TASK_RATE},
    };

    //Initialisation
    system_init ();
    navswitch_init ();
    tinygl_init (DISPLAY_TASK_RATE);
    init_positions(&player1);
    task_schedule (tasks, ARRAY_SIZE (tasks));

    return 0;
}
