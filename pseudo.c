//TODO: add appropriate header files
#include "system.h"
#include "pio.h"
#include "button.h"
#include "tinygl.h"
#include "../fonts/font3x5_1.h"

//My (George Khella) header files
#include "move.h"
#include "collision.h"
#include "hashmap.h"

/* ISSUES/CONCERNS:
 *     Due to the way collision checking works, sandbag struct cannot be
 *     turned into an array. However, I have made a "trench" which is an
 *     array of sandbags. The method should be completely working.
 *
 *     For efficiency reasons, will need to use a spatial hash map
 *     (i.e. hashtable of occupied coords) to speed up collision checking:
 *     from O(n) to O(1).
*/

/** RESOLVED ISSUES:
 * Both trenches captured --> Defeat
 * Previous player pointer --> doubly-linked list
 * Player selection --> Attacker, Defender
 * Players at same coord --> Christmas Truce
 * Knowin sandbags above, below and right/left of current sandbag for collision checking
 * 
 **/
 
/** COMPLETED TASKS:
 * Player collision check
 * Sandbag collision check w/ hashmap
 * Trench/sandbag initialise
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
 * Fix hash table collision bug 
 * Player collision check -- done! (efficiently) :)
 * Arrays for sandbags -- cancelled, see ISSUES/CONCERNS
 * Trench/sandbag initialisation -- done!
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

//Trench and sandbag related values
#define TRENCH_DEPTH 2
#define SANDBAG_HEALTH 2


typedef struct sandbag_s SandBag;
typedef struct bullet_s Bullet;
typedef struct player_s Player;


//Struct for sandbag or 2 arrays mapping each sandbag to appropriate coord (row, col)?
//--->arrays
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
    SandBag sandbags[NUM_COLS*2];
};

struct bullet_s {
    uint8_t x;
    uint8_t y;
    Player* target;
};

/*Hash table size is calculated as follows:
 (Num_Cols * Trench_depth) * displays * 2 + 1
 multiply by 2 is to have a load factor of 0.5 and add 1 to have a prime
 table size to reduce collision chances, so in our case we have:
 2 displays so multiplier is 4. 
 Note: hash_table_size here is 57
*/
static uint8_t hash_table_size = NUM_COLS * TRENCH_DEPTH * 4 + 1;
static SandBag hash_table[hash_table_size];


/*Hash function that takes an x and y coord multiplies each one by an 
 arbitrarily chosen number then computes the result modulo hash table size*/
uint8_t hash(uint8_t x, uint8_t y)
{
    /*This formula gives different hashes for our scenario,
    The 'magic' numbers here are arbitrary, other numbers can be used
    */
    uint8_t number = ((x*3 + y*4) << 3) % hash_table_size;

    return number;
}

//Add item to hash table using Open Addressing with Linear Probing
//Only need sandbags in hashtable so player parameter non-existent
void hash_add(SandBag sandbag)
{
    uint8_t hash_slot = hash(sandbag.x, sandbag.y);
    hash_table[hash_slot] = sandbag;
}

//Check whether the hash table contains a sandbag at current players coord.
//Returns NULL if no sandbag at player's coord
SandBag hash_contains(Player player, Bullet bullet)
{
    player ? (Player item = player) : (Bullet item = bullet);
    
    uint8_t hash_slot = hash(item.x, item.y);
    SandBag sandbag = hash_table[hash_slot];

    if(sandbag) {
        if (sandbag.x == item.x && sandbag.y == item.y) {
            return sandbag;
        } 
    
    
    return NULL;
}

Bullet* create_bullet(uint8_t x_pos, uint8_t y_pos, Player target)
{
    Bullet* bullet = {x_pos, y_pos, target};
    /*  bullet.x = x_pos;
        bullet.y = y_pos;
        bullet.target = target;
    */

    return bullet;
}


void damage_sandbag(SandBag* sandbag)
{
    sandbag.health--;
    sandbag.health == 0 ? (sandbag = NULL) : lower brightness of that sandbag;
}

//Check if player collided with a sandbag
//Return 0, no collision
//Return 1, collision with friendly sandbag, collision resolved
//Return 2, collision with enemy sandbag
static uint8_t sandbag_collision (Player* player, uint8_t move_type)
{
    SandBag us = player.sandbags;
    (SandBag them = ) player.next != NULL ? player.next : player.prev;

    /*Create a dummy variable to hold the player's new positions if 
      desire movemet were to occur. */
    Player new = player;
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
    new.y = y_pos;
    new.x = x_pos;

    //Collision detection O(1) way...
    SandBag sandbag = hash_contains(new);
    if (!sandbag) {
        return 0;
    } else if (sandbag) {
        if (sandbag.parent != player) {
            return 2;
        } else {
            if (move_type == UP) {
                //If sandbag above doesn't exist
                if !us[sandbag.x].y{
                    move_up(player);
                    move_up(player);

                    return 1;
                } else {
                    move_up(player);
                    move_up(player);
                    move_up(player);

                    return 1;
                }
            } else if (move_type == DOWN) {
                if(player.y == 2 || player.y == NUM_ROWS - 2) {
                    move_down(player);
                    move_down(player);

                    return 1;
                } else {
                    move_down(player);
                    move_down(player);
                    move_down(player);

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
static uint8_t player_collision_check (Player* player)
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
static uint8_t bullet_collision(Bullet* bullet)
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

//Need to shorten this function...a lot!
//Handles shooting event
static void shoot (Player* player)
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
    uint8_t i = 0;
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
static void init_positions (Player* player)
{
    while (player) {
        player.x = (uint8_t) (num_columns / 2); //Center of led matrix
        player.y = (player.next != NULL) ? 0 : num_rows*2;

        player = player.next;
    }

    player = player.prev;

    //TODO: Initialise trenches
    SandBag trench[TRENCH_DEPTH];

    uint8_t i = 0;
    uint8_t x_pos = 0;
    uint8_t y_pos = 1;

    while (player) {
        while (i < TRENCH_DEPTH) {
            if(x_pos > NUM_COLS) {
                x_pos = 0;
                y_pos++;
            }
            trench[i].x = x_pos;
            trench[i].y = y_pos;
            trench[i].health = SANDBAG_HEALTH;
            trench[i].parent = player;
            hash_add(trench[i]);
            x_pos++;
        }
        player.sandbags = trench;
        player = player.next;
    }
}

//Movement functions look like the should be modular (imo).
//Refer to the image if confused about how the coord system works here
void move_up(Player* player)
{
    if(player.next != NULL) {
        player.y < NUM_ROWS ? player.y++ : 0;
    } else {
        player.y > 0 ? player.y-- : 0;
    }
}

void move_down(Player* player)
{
    //Check if player1 or player2
    if(player.next != NULL) {
        player.y > 0 ? player1.y--:0;
    } else {
        player.y < NUM_ROWS ? player.y++:0;
    }
}

void move_left(Player* player)
{
    //Check player is not at left edge of led matrix
    player.x > 0 ? player.x--:0;
}

void move_right(Player* player)
{
    //Check player is not at right edge of led matrix
    player.x < NUM_COLS ? player.x++:0;
}

//Checks for game events (e.g. movement) and calls neccessary
//functions to handle them
void run_game(void)
{
    uint8_t col_type = 0;

    if (navswitch_push_event_p (NAVSWITCHT_NORTH)) {
        col_type = sandbag_collision(player, UP);
        (col_type == 0) ? move_up(player) : 0;
    } else if (navswitch_push_event_p (NAVSWITCHT_SOUTH)) {
        col_type = sandbag_collision(player, DOWN);
        (col_type == 0) ? move_down(player) : 0;
    } else if (navswitch_push_event_p (NAVSWITCHT_WEST)) {
        col_type = sandbag_collision(player, LEFT);
        (col_type == 0) ? move_left(player) : 0;
    } else if (navswitch_push_event_p (NAVSWITCHT_EAST)) {
        col_type = sandbag_collision(player, RIGHT);
        (col_type == 0) ? move_right(player) : 0;
    } else if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
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
    
    return 0;
}
