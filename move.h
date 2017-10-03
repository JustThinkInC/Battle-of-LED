/* Description: Header files of player movement functions.
 * Author: George Khella
 * Date: September 2017
 */


#include "pio.h"
#include "struct_init.h"

#ifndef MOVE_H
#define MOVE_H

void move_up(Player* player);

void move_down(Player* player);

void move_left(Player* player);

void move_right(Player* player);
#endif
