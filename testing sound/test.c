#include "system.h"
#include "tinygl.h"
#include "task.h"
#include "navswitch.h"
#include "led.h"
#include "tweeter.h"
#include "mmelody.h"
#include "pio.h"

#define PIEZO1_PIO PIO_DEFINE (PORT_D, 4)
#define PIEZO2_PIO PIO_DEFINE (PORT_D, 6)


#define TWEETER_TASK_RATE 5000
#define TUNE_TASK_RATE 100
#define TUNE_BPM_RATE 200
#define NAVSWITCH_TASK_RATE 100

static tweeter_scale_t scale_table[] = TWEETER_SCALE_TABLE (TWEETER_TASK_RATE);
static tweeter_t tweeter;
static mmelody_t melody;
static mmelody_obj_t melody_info;
static tweeter_obj_t tweeter_info;


static const char christmas_tune[] =
{
#include "merry_christmas.mmel"
};


static void tweeter_task_init (void)
{
    tweeter = tweeter_init (&tweeter_info, TWEETER_TASK_RATE, scale_table);

    pio_config_set (PIEZO1_PIO, PIO_OUTPUT_LOW);
    pio_config_set (PIEZO2_PIO, PIO_OUTPUT_LOW);

}


static void tweeter_task (__unused__ void *data)
{
    bool state;

    state = tweeter_update (tweeter);

    pio_output_set (PIEZO1_PIO, state);
    pio_output_set (PIEZO2_PIO, !state);

}


static void tune_task_init (void)
{
    melody = mmelody_init (&melody_info, TUNE_TASK_RATE, 
			   (mmelody_callback_t) tweeter_note_play, tweeter);

    mmelody_speed_set (melody, TUNE_BPM_RATE);
}


static void tune_task (__unused__ void *data)
{
    mmelody_update (melody);
}


static void navswitch_task (__unused__ void *data)
{
    
    navswitch_update ();
    
    if (navswitch_push_event_p (NAVSWITCH_PUSH))
    {
        mmelody_play (melody, christmas_tune);
    }
}


int
main (void)
{
    task_t tasks[] =
    {
        {.func = tweeter_task, .period = TASK_RATE / TWEETER_TASK_RATE,
         .data = 0},
        {.func = tune_task, .period = TASK_RATE / TUNE_TASK_RATE,
         .data = 0},
        {.func = navswitch_task, .period = TASK_RATE / NAVSWITCH_TASK_RATE,
         .data = 0},
    };


    system_init ();

    tweeter_task_init ();
    tune_task_init ();

    task_schedule (tasks, ARRAY_SIZE (tasks));
    return 0;
}
