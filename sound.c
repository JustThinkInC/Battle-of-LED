#include "stystem.h"
#include "pio.h"
#include "sound.h"

#define PIEZO1_PIO PIO_DEFINE (PORT_D, 4)
#define PIEZO2_PIO PIO_DEFINE (PORT_D, 6)

void sound_init (void) {
    
    pio_config_set (PIEZO1_PIO, PIO_OUTPUT_LOW);
    pio_config_set (PIEZO2_PIO, PIO_OUTPUT_HIGH);
    
}

void beep (void) {
    
    pio_output_toggle (PIEZO1_PIO);
    pio_output_toggle (PIEZO2_PIO);
    
}
