#include "sound.h"
#include "tweeter.h"
#include "mmelody.h"
#include "pio.h"

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
