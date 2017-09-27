//TODO: add appropriate header files
#include "system.h"
#include "pio.h"
#include "button.h"
#include "tinygl.h"
#include "../fonts/font3x5_1.h"

/* ISSUES: What happens if both capture each other's trench?
           Stalemate? Restart? Force players to recapture their trench
           then attempt to capture enemy's trench? Both sides victor?
           Both sides defeated?-->curently settling on this
           *
           *Not feeling too comfortable with current sandbag implementation.
           * Structs are feeling clunky in this scenario...
           *
           * How to point to previous player i.e. player1? Doubly-linked
           * list seems just a lazy option for this 'simple' scenario
           *
           * Need to find how to better initialise bullets instead of having
           * a create_bullet() call per if conidition.
           *
           * What happens if two players at same point at same instance and
           * have not fired at each other?
*/

/* IDEAS: Maybe have LED1 (blue led) flash as soon as opponent in no
 *        man's land, with the flash rate proportional to distance from
 *        your trench?
 *        Easter egg?
 *        Selection by country? (use abbrvs since flags need colours)
 *        Selection by year and/or country? higher year->tougher defence->slightly improved DPS
 *        CURRENT: Selection by Attacker/Defender
*/

/* TODO:
 * Player collision check
 * Arrays for sandbags
 * Game function for task scheduler
 * IR recieve&send
 * Player1 player2 check
 * UI Selection
 * Sound
 */

typedef struct sandbag_s SandBags;
typedef struct bullet_s Bullet;
typedef struct player_s Player;

//Struct for sandbag or 2 arrays mapping each sandbag to appropriate coord (row, col)?
struct sandbag_s {
    uint8_t health;
    SandBags* next_sandbag;
};

struct player_s {
    uint8_t x;
    uint8_t y;
    //uint8_t starter; //This acts as a bool to determine who started
    Player* next; //maybe this instead of starter
    Player* prev;
};

struct bullet_s {
    uint8_t y;
    Player* target;
}



Bullet* create_bullet(uint8_t x_pos, uint8_t y_pos, Player target)
{
    Bullet* bullet = malloc(sizeof(Bullet));//um no malloc in avr, will change
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

//Check for collisions between bullet other game objects
static int collision(Player target, Bullet bullet)
{
    uint8_t sandbag_number;
    if(target.x == bullet.x && target.y == bullet.y) {
        return 1;
    } else {
        while(sandbag_number < target_sandbags) {
            if(target_sandbags[sandbag_number].x == bullet.x &&
                    target_sandbags[sandbag_number].y == bullet.y) {
                delete(bullet);
                damage_sandbag(SandBags* sandbag);

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
        Bullet* bullet = create_bullet(player.x, player.y+1, player.prev); //previos pointer?!
    }

    while(bullet) {
        //If player1 then move bullet up else down (from p1 perspective!)
        player.next != null ? bullet.y++ : bulley.y--;
        collided = collision(player.target);
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
    if(player1) {
        player1.y < NUM_ROWS ? player1.y++ : 0;
    } else {
        player2.y > 0 ? player2.y-- : 0;
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
    if navswitch_push_event_p (NAVSWITCHT_NORTH) {
        move_up(player);
        player_collision_check () ? end_game : 0;
    } else if navswitch_push_event_p (NAVSWITCHT_SOUTH) {
        move_down(player);
        player_collision_check () ? end_game() : 0;
    } else if navswitch_push_event_p (NAVSWITCHT_WEST) {
        move_left(player);
        player_collision_check () ? end_game() : 0;
    } else if navswitch_push_event_p (NAVSWITCHT_EAST) {
        move_right(player);
        player_collision_check () ? end_game() : 0;
    } else if navswitch_push_event_p (NAVSWITCH_PUSH) {
        shoot(player);
    }
}

int main(void)
{
    //TODO: Initialisation
    init_positions ();

    //TODO: TASK SCHEDULER
}
