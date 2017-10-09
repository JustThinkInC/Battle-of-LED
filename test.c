#include "system.h"
#include "task.h"
#include "navswitch.h"
#include "tinygl.h"

#define NAVSWITCH_TASK_RATE 100
#define DISPLAY_TASK_RATE 250
#define MAX_Y 6
#define MAX_X 4
#define BAG_NUM 14

typedef struct sandbag_s {
    uint8_t health;
    tinygl_point_t pos;
} Sandbag;

typedef struct player_s {
    int health;
    Sandbag playerBags[BAG_NUM];
    tinygl_point_t pos;
    tinygl_point_t old;
} Player;

Player p1;

static void display_task (__unused__ void *data)
{
    tinygl_update ();
    
    tinygl_draw_point (p1.old, 0);
    tinygl_draw_point (p1.pos, 1);
    
    uint8_t i;
    for (i = 0; i < BAG_NUM; i++) {
        if (p1.playerBags[i].health < 1) {
            tinygl_draw_point (p1.playerBags[i].pos, 0);
        } else {
            tinygl_draw_point (p1.playerBags[i].pos, 1);
        }
    }
}

static void navswitch_task (__unused__ void *data)
{
    navswitch_update ();

    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        p1.old = p1.pos;
        if (p1.pos.y > 0){
            p1.pos.y--;
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_SOUTH)){
        p1.old = p1.pos;
        if (p1.pos.y < MAX_Y){
            p1.pos.y++;
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_WEST)) {
        p1.old = p1.pos;
        if (p1.pos.x > 0){
            p1.pos.x--;
        }
    }
    
    if (navswitch_push_event_p (NAVSWITCH_EAST)){
        p1.old = p1.pos;
        if (p1.pos.x < MAX_X){
            p1.pos.x++;
        }
    }

}

int main (void)
{
    
    task_t tasks[] =
    {
        {.func = display_task, .period = TASK_RATE / DISPLAY_TASK_RATE}, 
        {.func = navswitch_task, .period = TASK_RATE / NAVSWITCH_TASK_RATE},
    };
    
    p1.pos.x = 4;
    p1.pos.y = 3;

    system_init ();
    navswitch_init ();
    tinygl_init (DISPLAY_TASK_RATE);
    
    uint8_t i;
    for (i = 0; i < BAG_NUM; i++) {
        Sandbag bag;
        bag.pos.y = i;
        if (i < 7) {
            bag.pos.x = 3;
        } else {
            bag.pos.x = 2;
        }
        bag.health = 2;
        p1.playerBags[i] = bag;
    }
    
    /*
    tinygl_point_t bullet;
    bullet.x = 0;
    bullet.y = 0;
    if (bullet.x > 0) {
        bullet_old = bullet;
        bullet.x--;
        tinygl_draw_point (bullet_old, 0);
        tinygl_draw_point (bullet, 1);
    } else {
        tinygl_draw_point (bullet, 0);
    } 
     
    if (navswitch_push_event_p (NAVSWITCH_PUSH)){
        bullet = point;
    } 
    */
        
    task_schedule (tasks, ARRAY_SIZE (tasks));
        
    return 0;
}
