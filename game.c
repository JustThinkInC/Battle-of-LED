/** @file    game.c
    @authors George Khella & Theo Harbers
    @date    October 2017.
    @brief   BATTLE OF LED 1914 game code.
    @version 1.1
*/

/* UPDATE: AI added for single player support. */

/*
   This implements a top down shooter game. Players attempt to form a
   a gap in the opponent's trench by shooting sandbags. Players then try
   to reach the end without being shot by the opponent, if they do so
   successfully, they have won. Players only have 1 life and 1 hp,
   sandbags have 2 hp each. Bullets are plentiful, but spamming them
   will make an unarmed player.
*/


//Supplied header files, credits to their respective owners
#include "stdlib.h"
#include "system.h"
#include "pio.h"
#include "tinygl.h"
#include "task.h"
#include "drivers/fonts/font3x5_1.h"
#include "navswitch.h"
#include "led.h"
#include "ir_uart.h"
#include "tweeter.h"
#include "mmelody.h"
#include "drivers/button.h"
#include "led.h"



//George Khella's header files
#include "own/struct_init.h"
#include "own/hashmap.h"
#include "own/move.h"
#include "own/collision.h"

//Theo Harbers' Header files
#include "own/sound.h"


/**NOTES:
   1)move up = y--, move down = y++
   2)We could reuse bullets to have an infite number, but we decided
     its more fun (and realistic!) if you have to be slightly careful :)
   3)Instead of an arcady bullet speed where you actually see the bullet,
     we think its more fun if you can't see them and have to actually
     predict the opponent's actions
   4)Regarding IR characters, U=UP, D=Down, L=LEFT, R=RIGHT, V=VICTORY,
     T=TRUCE and numbers are the sandbag slot numbers.
**/

//Polling rate of tasks in Hz
#define DISPLAY_TASK_RATE 300
#define GAME_TASK_RATE 100
#define BOT_RATE 25

Bullet bulletPool[MAX_NUM_BULLETS]; //The bullet pool
uint8_t firstFree = 0; //First free slot in the bullet pool
static bool game_over = 0; //Used to check if game has ended.
static bool show_menu = 1; //Used to check if menu should be displayed
static bool singlePlayer = 0;

//Create the players
Player player1;
Player player2;

static const char* game_title = "BATTLE OF LED 1914";

//This function updates the output state of the piezo IO pins
static void tweeter_task (__unused__ void *data)
{
    bool state;
    state = tweeter_update (tweeter);

    pio_output_set (PIEZO1_PIO, state);
    pio_output_set (PIEZO2_PIO, state);
}

//This function simply updates the melody
static void tune_task (__unused__ void *data)
{
    mmelody_update (melody);
}

//Displays a menu with a given message
void display_menu (const char* msg)
{
    tinygl_clear();
    if(show_menu || game_over) {
        tinygl_text(msg);
        mmelody_play (melody, menu_tune);
    }
    tinygl_update();
}

//Clears all drawings, then redraws all game objects
void draw(Player* player)
{
    //Clear any melody playing and reset the display
    mmelody_play (melody, " ");
    tinygl_clear();

    //Draw only if the game is running
    if (!game_over && !show_menu) {
        tinygl_draw_point (player->pos, 1);
        tinygl_draw_point (player->next->pos, 1);
        //Draw the sandbags, if they aren't destroyed
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
        //Check that a bullet has been made and the bullet does exist
        if (firstFree > 0 && !&bulletPool[firstFree-1].inactive) {
            Bullet bullet = bulletPool[firstFree-1];
            tinygl_draw_point(bullet.pos, 1);
        }
    }
}


/*End the game, taking in the winning player as the param
  Each player recieves a message based on their outcome.
*/
void end_game(Player* player)
{
    if (player->next != NULL) {
        game_over = 1;
        display_menu("VICTORY :)");
        mmelody_play (melody, victory_tune);
    } else {
        game_over = 1;
        display_menu("DEFEAT :(");
        mmelody_play (melody, defeat_tune);
    }
}


//Upon players colliding with each other, we make an explicit reference
//to a historical event :)
void christmas_truce(void)
{
    game_over = 1;
    const char* msg = "CHRISTMAS TRUCE 1914";
    display_menu(msg);
    mmelody_play (melody, christmas_tune);
}


/*This function creates a bullet if the bullet pool still has space
  To make things a little more fun, bullets are not reused if a player
  goes over pre-set capacity.*/
Bullet* create_bullet(uint8_t x_pos, uint8_t y_pos, Player* target)
{
    Bullet* bullet = NULL;
    firstFree = 0;
    if (firstFree < MAX_NUM_BULLETS) {
        bullet = &bulletPool[firstFree];
        firstFree++;
        bullet->pos.x = x_pos;
        bullet->pos.y = y_pos;
        bullet->target = target;
        bullet->inactive = 0;
    }


    return bullet;
}


/*Handles shooting, enemy sandbags lose 1hp if hit, friendly sandbags
  ignored, game over if opponent hit by bullet.
*/
static void shoot (Player* player)
{
    Bullet* bullet;
    //If player1 fired the shot
    if(player->next != NULL) {
        bullet = create_bullet(player->pos.x, player->pos.y, player->next);
    } else {
        bullet = create_bullet(player->pos.x, player->pos.y, player->prev);
    }

    Player* target = bullet->target;

    while(!bullet->inactive) {
        player->next != NULL ? bullet->pos.y-- : bullet->pos.y++;
        //Hash contains doesn't return pointer to sandbag, so we use a temp
        //to point to the real sandbag
        SandBag sandbag_temp = hash_contains(bullet->pos.x, bullet->pos.y);
        SandBag* sandbag = &target->sandbags[sandbag_temp.slot];
        draw(&player1);
        uint8_t y_pos = target->sandbags[0].pos.y;
        if (sandbag->health > 0 && sandbag->parent == target && bullet->pos.y == y_pos) {
            sandbag->health--;
            hash_add(*sandbag);
            bullet->inactive = 1;
            !singlePlayer ? ir_uart_putc((uint8_t)sandbag->slot) : 0;
            draw(&player1);
        } else if (bullet->pos.x == target->pos.x && bullet->pos.y == target->pos.y) {
            bullet = NULL;
            !singlePlayer ? ir_uart_putc('V') : 0;

            end_game(player);
        } else if (bullet->pos.y >= LEDMAT_ROWS_NUM || bullet->pos.y < 0) {
            bullet->inactive = 1;
        }
    }
}

//This function simply updates the display
static void display_task (__unused__ void *data)
{
    tinygl_update();
}

/*This task recieves a character (if one has been sent!) and updates
*the game accordingly
*/
static void ir_recieve_task (__unused__ void *data)
{
    uint8_t col_type = 0;

    char action = '0'; //The action to be taken (if any!)
    if (ir_uart_read_ready_p()) {
        action = ir_uart_getc();
    }

    if (action >= 0 && action <= SANDBAG_NUM) {
        Player* player = &player1;
        player->sandbags[SANDBAG_NUM - action - 1].health--;
        hash_add(player1.sandbags[SANDBAG_NUM - action - 1]);
        draw(&player1);
    } else {
        switch (action) {
        case 'U':
            col_type = sandbag_collision(&player2, UP);
            (col_type == 0) ? move_up(&player2, 1) : 0;
            draw (&player1);
            break;
        case 'D':
            col_type = sandbag_collision(&player2, DOWN);
            (col_type == 0) ? move_down(&player2, 1) : 0;
            draw (&player1);
            break;
        case 'R':
            col_type = sandbag_collision(&player2, RIGHT);
            (col_type == 0) ? move_right(&player2) : 0;
            draw (&player1);
            break;
        case 'L':
            col_type = sandbag_collision(&player2, LEFT);
            (col_type == 0) ? move_left(&player2) : 0;
            draw (&player1);
            break;
        case 'T':
            christmas_truce();
            break;
        //Recieving V (victory) means we have lost
        case 'V':
            end_game(&player2);
        }
    }
}

//Initialise player and sandbag positions
void init_positions (Player* player)
{
    //Set next and prev player pointers
    player1.next = &player2;
    player2.prev = &player1;

    uint8_t init = 2;
    while (init) {
        player->pos.x = ((LEDMAT_COLS_NUM-1) / 2); //Center of led matrix
        player->pos.y = (player->next != NULL) ? LEDMAT_ROWS_NUM-1 : 0;
        init > 1 ? player = player->next : 0;
        init--;
    }

    player = player->prev; //Back to player1

    uint8_t i = 0;
    uint8_t x_pos = 0;
    //Make trench on second to last row
    uint8_t y_pos = LEDMAT_ROWS_NUM - 2;

    //Initialising the sandbags
    while (player != NULL) {
        while (i < SANDBAG_NUM) {
            //Resetting x if we go over the maximum display width
            if(x_pos > LEDMAT_COLS_NUM-1) {
                x_pos = 0;
                y_pos--;
            }
            y_pos = (player->next == NULL) ? 1 : y_pos;
            player->sandbags[i].pos.x = x_pos;
            player->sandbags[i].pos.y = y_pos;
            player->sandbags[i].health = SANDBAG_HEALTH;
            player->sandbags[i].parent = player;
            player->sandbags[i].slot = i;
            hash_add(player->sandbags[i]);
            x_pos++;
            i++;
        }
        player = player->next;
        i = 0;
    }
    draw(&player1);
    tinygl_update();
}


void play_bot (__unused__ void *data)
{
    //Implement random number generator
    //40% chance of shooting, 20% chance move_up, 10% chance other movement
    //10% chance do nothing
    static bool state = 0;
    if (singlePlayer && !game_over && !show_menu) {
        uint8_t col_type = 0;
        uint8_t num = rand() % 255; //bot is really fast, so using a max range
        
        if (num < 4) {
            shoot(&player2);
        } else if (num == 4) {
            col_type = sandbag_collision(&player2, UP);
            (col_type == 0) ? move_up(&player2, 1) : 0;
            draw (&player1);
        } else if (num == 5 || num == 6) {
            col_type = sandbag_collision(&player2, DOWN);
            (col_type == 0) ? move_down(&player2, 1) : 0;
            draw (&player1);
        } else if (num == 7) {
            col_type = sandbag_collision(&player2, LEFT);
            (col_type == 0) ? move_left(&player2) : 0;
            draw (&player1);
        } else if (num == 8) {
            col_type = sandbag_collision(&player2, RIGHT);
            (col_type == 0) ? move_right(&player2) : 0;
            draw (&player1);
        }
        
    } else if (show_menu) {
        button_update();
        if (button_push_event_p(0)) {
            led_set(LED1, !state);
            state = !state;
            singlePlayer = !singlePlayer;
        }
    }

}

/*Checks for game events (e.g. movement) and calls neccessary functions
  to handle them
  Note that movements appear to be inverse of what they are to different
  players hence the inversion when sending via IR.
*/
static void run_game_task (__unused__ void *data)
{
    uint8_t col_type = 0;
    navswitch_update();
    //If we are at the main menu, then pushing down starts the game
    if (show_menu && navswitch_push_event_p (NAVSWITCH_PUSH)) {
        show_menu = 0;
        game_over = 0;
        draw(&player1);
        //If game has ended, upon push down, we return to the main menu
    } else if (game_over && navswitch_push_event_p (NAVSWITCH_PUSH)) {
        init_positions (&player1);
        firstFree = 0;
        game_over = 0;
        show_menu = 1;
        display_menu(game_title);
        //Game is running, we now check for the player's actions
    } else if (!game_over && !show_menu) {
        if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
            col_type = sandbag_collision(&player1, UP);
            (col_type == 0) ? move_up(&player1, 1) : 0;
            draw(&player1);
            !singlePlayer ? ir_uart_putc('D') : 0;
        } else if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
            col_type = sandbag_collision(&player1, DOWN);
            (col_type == 0) ? move_down(&player1, 1) : 0;
            draw(&player1);
            !singlePlayer ? ir_uart_putc('U') : 0;
        } else if (navswitch_push_event_p (NAVSWITCH_WEST)) {
            col_type = sandbag_collision(&player1, LEFT);
            (col_type == 0) ? move_left(&player1) : 0;
            draw(&player1);
            !singlePlayer ? ir_uart_putc('R') : 0;
        } else if (navswitch_push_event_p (NAVSWITCH_EAST)) {
            col_type = sandbag_collision(&player1, RIGHT);
            (col_type == 0) ? move_right(&player1) : 0;
            draw(&player1);
            !singlePlayer ? ir_uart_putc('L') : 0;
        } else if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
            shoot(&player1);
        }

        //Check if the players have collided, if so, *spoiler alert*
        //delcare the 1914 Christmas Truce
        col_type = player_collision_check (&player1);
        if(col_type) {
            ir_uart_putc('T');
            christmas_truce();
            //Check if we have captured the enemy trench
        } else if (player1.pos.y == 0) {
            //Inform opponent we have won
            ir_uart_putc('V');
            end_game(&player1);
        } else if (player2.pos.y == 6) {
            end_game(&player2);
        }
    }
}


int main(void)
{
    task_t tasks[] = {
        {.func = tweeter_task, .period = TASK_RATE / TWEETER_TASK_RATE},
        {.func = tune_task, .period = TASK_RATE / TUNE_TASK_RATE},
        {.func = display_task, .period = TASK_RATE / DISPLAY_TASK_RATE},
        {.func = run_game_task, .period = TASK_RATE / GAME_TASK_RATE},
        {.func = ir_recieve_task, .period = TASK_RATE / GAME_TASK_RATE},
        {.func = play_bot, .period = TASK_RATE / BOT_RATE},
    };

    //Initialisation
    system_init ();
    tweeter_task_init ();
    tune_task_init ();
    navswitch_init ();
    ir_uart_init();
    tinygl_init (DISPLAY_TASK_RATE);
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (10);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set(TINYGL_TEXT_DIR_ROTATE);
    init_positions(&player1);
    //Show the main menu to the player
    display_menu(game_title);



    task_schedule (tasks, ARRAY_SIZE (tasks));


    return 0;
}
