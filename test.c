#include "system.h"
#include "task.h"
#include "navswitch.h"
#include "tinygl.h"
#include "ir_uart.h"

#define NAVSWITCH_TASK_RATE 100
#define DISPLAY_TASK_RATE 250
#define MAX_Y 6
#define MAX_X 4

enum {NORTH, SOUTH, WEST, EAST};

typedef struct player_s {
    int health;
    tinygl_point_t pos;
    tinygl_point_t old;
} Player;

Player p1;
Player p2;

void p2_movement (uint8_t movement) {
    
    if (movement == NORTH) {
        p2.old = p2.pos;
        if (p2.pos.y > 0){
            p2.pos.y--;
        }
    }
    if (movement == SOUTH){
        p2.old = p2.pos;
        if (p2.pos.y < MAX_Y){
            p2.pos.y++;
        }
    }
    if (movement == WEST) {
        p2.old = p2.pos;
        if (p2.pos.x > 0){
            p2.pos.x--;
        }
    }
    if (movement == EAST){
        p2.old = p2.pos;
        if (p2.pos.x < MAX_X){
            p2.pos.x++;
            ir_uart_putc (EAST);
        }
    }
}

static void display_task (__unused__ void *data)
{
    tinygl_update ();
    
    if (ir_uart_read_ready_p()) {
        uint8_t movement = ir_uart_getc ();
        p2_movement (movement);
    }

    
    tinygl_draw_point (p1.old, 0);
    tinygl_draw_point (p1.pos, 1);
    
    tinygl_draw_point (p2.old, 0);
    tinygl_draw_point (p2.pos, 1);
}

static void navswitch_task (__unused__ void *data)
{
    navswitch_update ();

    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        p1.old = p1.pos;
        if (p1.pos.y > 0){
            p1.pos.y--;
            ir_uart_putc (NORTH);
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_SOUTH)){
        p1.old = p1.pos;
        if (p1.pos.y < MAX_Y){
            p1.pos.y++;
            ir_uart_putc (SOUTH);
        }
    }
    if (navswitch_push_event_p (NAVSWITCH_WEST)) {
        p1.old = p1.pos;
        if (p1.pos.x > 0){
            p1.pos.x--;
            ir_uart_putc (WEST);
        }
    }
    
    if (navswitch_push_event_p (NAVSWITCH_EAST)){
        p1.old = p1.pos;
        if (p1.pos.x < MAX_X){
            p1.pos.x++;
            ir_uart_putc (EAST);
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
    
    p2.pos.x = 0;
    p2.pos.y = 3;

    system_init ();
    navswitch_init ();
    tinygl_init (DISPLAY_TASK_RATE);
    ir_uart_init ();
    
    task_schedule (tasks, ARRAY_SIZE (tasks));
        
    return 0;
}
