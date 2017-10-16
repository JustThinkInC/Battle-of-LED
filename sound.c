#include "sound.h"
#include "tweeter.h"
#include "mmelody.h"
#include "pio.h"


tweeter_scale_t scale_table[] = TWEETER_SCALE_TABLE (TWEETER_TASK_RATE); 


const char menu_tune[] = {
#include "the_front_short.mmel"
    " :"
};

const char christmas_tune[] = {
#include "merry_christmas.mmel"
    " :"
};

const char victory_tune[] = {
    "BBBA// AAAB// :"
};

const char defeat_tune[] = {
    "F///G//:"
};

void tweeter_task_init (void)
{
    tweeter = tweeter_init (&tweeter_info, TWEETER_TASK_RATE, scale_table);

    pio_config_set (PIEZO1_PIO, PIO_OUTPUT_LOW);
    pio_config_set (PIEZO2_PIO, PIO_OUTPUT_LOW);

}

void tune_task_init (void)
{
    melody = mmelody_init (&melody_info, TUNE_TASK_RATE,
                           (mmelody_callback_t) tweeter_note_play, tweeter);

    mmelody_speed_set (melody, TUNE_BPM_RATE);
}
