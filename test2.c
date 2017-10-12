#include "system.h"
#include "task.h"
#include "navswitch.h"
#include "tinygl.h"

#define NAVSWITCH_TASK_RATE 100
#define DISPLAY_TASK_RATE 250

static void display_task (__unused__ void *data)
{
    tinygl_point_t pos;
    pos.x = 2;
    pos.y = 3;
    
    tinygl_update ();
    
    tinygl_draw_point (pos, 1);
    
}

static void navswitch_task (__unused__ void *data)
{
    navswitch_update ();

    if (navswitch_push_event_p (NAVSWITCH_PUSH)) {
        beep();
    }
}


int main (void)
{
    task_t tasks[] =
    {
        {.func = display_task, .period = TASK_RATE / DISPLAY_TASK_RATE}, 
        {.func = navswitch_task, .period = TASK_RATE / NAVSWITCH_TASK_RATE},
    };
    
    system_init ();
    navswitch_init ();
    tinygl_init (DISPLAY_TASK_RATE);
    
    sound_init ();
    
    task_schedule (tasks, ARRAY_SIZE (tasks));
        
    return 0;
}
