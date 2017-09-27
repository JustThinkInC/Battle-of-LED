//TODO: add appropriate header files
#include "system.h"
#include "pio.h"
#include "button.h"
#include "tinygl.h"
#include "move.h"
#include "../fonts/font3x5_1.h"

/* ISSUES/CONCERNS: 
 *     If a player moves up and happens to collide with first sandbag
 *     player.y needs to be incremented by 2. But what if second sandbag? 
 *     Then increment 1, how to know which one? Same issue with down.
*/

/** RESOLVED ISSUES:
 * Both trenches captured --> Defeat
 * Previous player pointer --> doubly-linked list
 * Player selection --> Attacker, Defender
 * Players at same coord --> Christmas Truce
 *
 **/

/** IDEAS: Maybe have LED1 (blue led) flash as soon as opponent in no
 *        man's land, with the flash rate proportional to distance from
 *        your trench?
 *        Easter egg? (Christmas Truce is one, maybe more if have time)
 *        Selection by year and/or country? higher year->tougher defence->slightly improved DPS
 *           -->CURRENT: Selection by Attacker/Defender maybe both
**/

/* TODO:
 * Player collision check
 * Arrays for sandbags
 * Trench/sandbag initialisation
 * Game function for task scheduler
 * IR recieve&send
 * Check wich board is P1
 * UI Selection
 * Sound
 */
 
//Polling rate of tasks in Hz
//Values may need adjustmenst after testing.
#defne DISPLAY_TASK_RATE 250
#define GAME_TASK_RATE 100
 
//Movement types and values
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define NONE 0

typedef struct sandbag_s SandBags;
typedef struct bullet_s Bullet;
typedef struct player_s Player;

//Struct for sandbag or 2 arrays mapping each sandbag to appropriate coord (row, col)?
//--->arrays
struct sandbag_s {
    uint8_t health;
    SandBags* next_sandbag;
};

struct player_s {
    uint8_t x;
    uint8_t y;
    Player* next;
    Player* prev;
    SandBags* sandbags;
};

struct bullet_s {
    uint8_t x;
    uint8_t y;
    Player* target;
}



Bullet* create_bullet(uint8_t x_pos, uint8_t y_pos, Player target)
{
    Bullet* bullet = malloc(sizeof(Bullet));//um no malloc in avr, need to change
    bullet.x = x_pos;
    bullet.y = y_pos;
    bullet.target = target;

    return bullet;
}


void damage_sandbag(SandBags* sandbag)
{
    sandbag.health--;
    sandbag.health == 0 ? delete(sandbag) : lower brightness of that sandbag;
}

//
static int sandbag_collision (Player player, uint8_t move_type) {
    
    SandBags us = player.sandbags;
    (SandBags them = ) player.next != NULL ? player.next : player.prev;
    
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

    //Psuedo code for collision
    while(sandbag <= us) {
       if(us[sandbag].x == x_pos && us[sandbag].y == y_pos) {
            if(move_type == UP) {
                first_row_destroyed ? (move_up(); move_up(); return 1): (move_up(); move_up(); move_up(); return 1);
            } else if (move_type == DOWN) {
                first_row_destroyed && !second_row_destroyed ? (move_down() ; move_down(); return 1) : 0;
                second_row_destroyed ? (move_down (); move_down(); return 1);
            } else if (move_type == LEFT) {
                left_sandbag_of_left_destroyed ? (move_left() : 0);
                return 1;
            } else if (move_type ==  RIGHT) {
                right_sandbag_of_right_destroyed ? (move_right() : 0);
                return 1;
            }
        } else if(them[sandbag].x == x_pos && them[sandbag].y == y_pos) {
            return 1;
        }
        sandbag++;
    }
    
    
    return 0;
} 

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

//Need to shorten this function...a lot!
//Handles shooting event
static void shoot (Player player)
{
    uint8_t collided = 0;

    //If player1 fired the shot
    if(player.next != NULL) {
        Bullet* bullet = create_bullet(player.x, player.y+1, player.next);
    } else {
        Bullet* bullet = create_bullet(player.x, player.y+1, player.prev);
    }

    while(bullet) {
        //If player1 then move bullet up else down (from p1 perspective!)
        player.next != null ? bullet.y++ : bulley.y--;
        collided = bullet_collision(bullet);
        if (!collided) {
            (bullet.y >= ROW_NUM || bullet.y <= 0) ? delete(bullet) : 0;
        }
    }
}



//Initialise ledmatrix
void led_init(void)
{
    //NOT PSUEDO, proper functioning code
    int i = 0;
    while (i <= LEDMAT_ROWS_NUM - 1) {
        pio_config_set(rows[i], PIO_OUTPUT_HIGH);
        i++;
    }
    i = 0;

    while (i <= LEDMAT_COLS_NUM - 1) {
        pio_config_set(cols[i], PIO_OUTPUT_HIGH);
        i++;
    }
}


//Initialise player positions and trenches
static void init_positions (void)
{
    while (player) {
        player.x = (uint8_t) (num_columns / 2); //Center of led matrix
        player.y = (player.next != NULL) ? 0 : num_rows*2;

        player = player.next;
    }
    //TODO: Initialise trenches
    while (trench) {
    for current_row and current_column:
        initialise sandbag;
    trench = trench.next;
}
}

//Movement functions look like the should be modular (imo).
//Refer to the image if confused about how the coord system works here
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
{
    //Check player is not at right edge of led matrix
    player.x < NUM_COLS ? player.x++:0;
}

//Checks for game events (e.g. movement) and calls neccessary
//functions to handle them
void run_game(void)
{
    uint8_t col_type = 0;
    if navswitch_push_event_p (NAVSWITCHT_NORTH) {
        col_type = sandbag_collision(player, UP);
        (col_type == 0) ? move_up(player) : 0;
    } else if navswitch_push_event_p (NAVSWITCHT_SOUTH) {
        col_type = sandbag_collision(player, DOWN);
        (col_type == 0) ? move_down(player) : 0;        
    } else if navswitch_push_event_p (NAVSWITCHT_WEST) {
        col_type = sandbag_collision(player, LEFT);
        (col_type == 0) ? move_left(player) : 0;
    } else if navswitch_push_event_p (NAVSWITCHT_EAST) {
        col_type = sandbag_collision(player, RIGHT);
        (col_type == 0) ? move_right(player) : 0;
    } else if navswitch_push_event_p (NAVSWITCH_PUSH) {
        shoot(player);
    }
    
    col_type = player_collision_check (player);
    col_type ? end_game() : 0;
}

int main(void)
{
    task_t tasks[] = {
        {.func = display_task, .period = TASK_RATE / DISPLAY_TASK_RATE},
        {.func = run_game, .period = TASK_RATE / GAME_TASK_RATE},
    };

    //TODO: Initialisation
    system_init();
    init_positions ();

    //TODO: TASK SCHEDULER
    task_schedule (tasks, ARRAY_SIZE (tasks));
}
