/** @file   sound.h
    @author Theo Harbers
    @date   October 2017
    @brief  Sound Header
*/

#ifndef SOUND_H
#define SOUND_H

#include "tweeter.h"
#include "mmelody.h"
#include "pio.h"

#define PIEZO1_PIO PIO_DEFINE (PORT_D, 4)
#define PIEZO2_PIO PIO_DEFINE (PORT_D, 6)

#define TWEETER_TASK_RATE 5000
#define TUNE_TASK_RATE 100
#define TUNE_BPM_RATE 200

static tweeter_scale_t scale_table[] = TWEETER_SCALE_TABLE (TWEETER_TASK_RATE);
static tweeter_t tweeter;
static mmelody_t melody;
static mmelody_obj_t melody_info;
static tweeter_obj_t tweeter_info;

static const char menu_tune[] = {
#include "the_front_short.mmel"
    " :"
};

static const char christmas_tune[] = {
#include "merry_christmas.mmel"
    " :"
};

static const char victory_tune[] = {
    "ACCGF:"
};

static const char defeat_tune[] = {
    "F///G//:"
};

// initialise tweeter task
static void tweeter_task_init (void)

// initialise tune task
static void tune_task_init (void)

#endif
