#include "system.h"
#include "stdlib.h"
#include "pio.h"
#include "tinygl.h"
#include "task.h"
#include "../fonts/font3x5_1.h"
#include "navswitch.h"
#include "led.h"
#include "ir_uart.h"

//George Khella's header files
#include "struct_init.h"
#include "hashmap.h"
#include "move.h"
#include "collision.h"

//Theo Harber's header files



/* ISSUES/CONCERNS:
 * RAM consumption is about 80% thanks to tinygl_text("") call
 * Bullet speed is realistic but not arcady. Which do we want?
*/

/* TODO:
 * IR recieve&send
 * Check wich board is P1
 * Sound
 */


//Note that move up = y--, move down = y++

//Polling rate of tasks in Hz
//Values may need adjustmenst after testing.
#define DISPLAY_TASK_RATE 250
#define GAME_TASK_RATE 100

Bullet bulletPool[MAX_NUM_BULLETS]; //The bullet pool
uint8_t firstFree = 0; //First free slot in the bullet pool

static bool state = 0; //If bullet hits something
static bool game_over = 0; //If game over
bool show_menu = 1;


//Create the players, we probably need to find another way to do this...
Player player1;
Player player2;


//Name I can think of is "Battle of LED", if you have another name tell me!
static const char* start_msg = "BATTLE OF LED 1914";


//This function displays a given message by tinygl
void display_menu (const char* p1_msg, const char* p2_msg)
{
    //TODO: When we have IR:
    // p1_ir_send("p2_msg");

    tinygl_clear();
    if(show_menu || game_over) {
        tinygl_text(p1_msg);
    } else {
        tinygl_point_t pos;
        pos.x = 0;
        pos.y = 0;
        tinygl_draw_message(p1_msg, pos, 1);
    }
    tinygl_update();
}


//This function draws all game objects (player, sandbags, bullets)
void draw(Player* player)
{
    tinygl_clear();
    uint8_t col_type = 0;
    char move = '0';
    if (ir_uart_read_ready_p()) {
        move = ir_uart_getc();
    }
    switch (move) {
    case 'U':
        col_type = sandbag_collision(&player2, UP);
        (col_type == 0) ? move_up(&player2, 10) : 0;
        break;
    case 'D':
        col_type = sandbag_collision(&player2, DOWN);
        (col_type == 0) ? move_down(&player2, 10) : 0;
        break;
    case 'R':
        col_type = sandbag_collision(&player2, RIGHT);
        (col_type == 0) ? move_right(&player2) : 0;
        break;
    case 'L':
        col_type = sandbag_collision(&player2, LEFT);
        (col_type == 0) ? move_left(&player2) : 0;
    }
    if (!game_over && !show_menu) {
        tinygl_draw_point (player->pos, 1);
        tinygl_draw_point (player->next->pos, 1);

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
            tinygl_update();
            timer_wait (GAME_TASK_RATE);
        }
    }
}


//This functions ends the game, taking in the winning player as the param
//Each player recieves a message based on their outcome
void end_game(Player* player)
{
    //TODO: End game
    if (player->next != NULL) {
        game_over = 1;
        display_menu("VICTORY :)" , "DEFEAT :(");

    } else {
        display_menu("DEFEAT :(", "VICTORY :)");
    }
    /*
    wait like 5 seconds then bring back to main menu;
    maybe play a tune for each player depending on the outcome
    --->Currently, I'm just making it so user has to click to get back to menu
    ----->this is posisbly better in case user missed the message?
    */

}


//Upon players colliding with each other, we make an explicit refernce
//to a historical event :)
void christmas_truce(void)
{
    //TODO: End game
    // play melody = "We wish you a Merry Christmas";
    // back to menu after melody ends;
    game_over = 1;
    const char* msg = "CHRISTMAS TRUCE, 1914";
    display_menu(msg, msg);
}


/*This function creates a bullet if the bullet pool still has space
  To make things a little more fun, bullets are not reused if a player
  goes over pre-set capacity.*/
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


/*Handles shooting, bullets are created, fired, and checked for collisions
 *Friendly sandbags ignored: bullets go above them.
 *Upon collision with enemy sandbag, that sandbag loses 1 hp (health point)
 *Game over if collided with target player
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
        player->next != NULL ? bullet->pos.y-- : bullet->pos.y--;

        //We need a temp (copy) of the sandbag, so we can make a pointer
        //to the real sandbag
        SandBag sandbag_temp = hash_contains(bullet->pos.x, bullet->pos.y);
        SandBag* sandbag = &target->sandbags[sandbag_temp.slot];

        draw(&player1);
        uint8_t y_pos = target->sandbags[0].pos.y;
        if (sandbag->health > 0 && sandbag->parent == target && bullet->pos.y == y_pos) {
            sandbag->health--;
            hash_add(*sandbag);
            bullet->inactive = 1;
            draw(&player1);
        } else if (bullet->pos.x == target->pos.x && bullet->pos.y == target->pos.y) {
            bullet = NULL;
            state = !state;
            led_set (LED1, state);
            end_game(&player1);
        } else if (bullet->pos.y >= LEDMAT_ROWS_NUM || bullet->pos.y < 0) {
            bullet->inactive = 1;
            state = !state;
            led_set (LED1, state);
        }
    }
}

//This function simply updates the display
static void display_task (__unused__ void *data)
{
    tinygl_update();
}


//Initialise player and sandbag positions
void init_positions (Player* player)
{
    //Set next and prev to avoid
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
    uint8_t y_pos = LEDMAT_ROWS_NUM - 2;

    while (player != NULL) {
        while (i < SANDBAG_NUM) {
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


/*Checks for game events (e.g. movement) and calls neccessary
functions to handle them
Note that movements appear to be inverse of what they are to different players
hence the inversion when sending via IR.
*/
static void run_game_task (__unused__ void *data)
{
    uint8_t col_type = 0;
    navswitch_update();

    if (show_menu && navswitch_push_event_p (NAVSWITCH_PUSH)) {
        show_menu = 0;
        game_over = 0;
        draw(&player1);

    } else if (game_over && navswitch_push_event_p (NAVSWITCH_PUSH)) {
        init_positions (&player1);
        game_over = 0;
        show_menu = 1;
        display_menu(start_msg,start_msg);

    } else if (!game_over && !show_menu) {
        if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
            col_type = sandbag_collision(&player1, UP);
            (col_type == 0) ? move_up(&player1, 10) : 0;
            draw(&player1);
            ir_uart_putc('D');
        } else if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
            col_type = sandbag_collision(&player1, DOWN);
            (col_type == 0) ? move_down(&player1, 10) : 0;
            draw(&player1);
            ir_uart_putc('U');
        } else if (navswitch_push_event_p (NAVSWITCH_WEST)) {
            col_type = sandbag_collision(&player1, LEFT);
            (col_type == 0) ? move_left(&player1) : 0;
            draw(&player1);
            ir_uart_putc('R');
        } else if (navswitch_push_event_p (NAVSWITCH_EAST)) {
            col_type = sandbag_collision(&player1, RIGHT);
            (col_type == 0) ? move_right(&player1) : 0;
            draw(&player1);
            ir_uart_putc('L');
        } else if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
            shoot(&player1);
        }

        col_type = player_collision_check (&player1);
        col_type ? christmas_truce() : 0;
        player1.pos.y == 0 ? end_game(&player1) : 0;
    }

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
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (10);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set(TINYGL_TEXT_DIR_ROTATE);
    init_positions(&player1);
    ir_uart_init();

    display_menu(start_msg, start_msg);

    task_schedule (tasks, ARRAY_SIZE (tasks));


    return 0;
}
