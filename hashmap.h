/* Description: Header file for the hash table holding sandbags.
 *              This allows for faster O(1) collision checking instead of
 *              O(n).
 * Author: George Khella
 * Date: October 2017
*/


#include "struct_init.h"
#include "pio.h"

#ifndef HASHMAP_H
#define HASHMAP_H
//Trench and sandbag related values
#define TRENCH_DEPTH 2

uint8_t hash(uint8_t x, uint8_t y);

//Add item to hash table using Open Addressing with Linear Probing
//Only need sandbags in hashtable so player parameter non-existent
void hash_add(SandBag sandbag);

//Check whether the hash table contains a sandbag at current players coord.
//Returns NULL if no sandbag at player's coord
SandBag hash_contains(uint8_t x, uint8_t y);
#endif

