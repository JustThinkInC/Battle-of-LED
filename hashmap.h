/** @file   hashmap.h
    @author George Khella
    @date   October 2017.
    @brief  Header file for sandbag hash table. Used for O(1) collision
            detection.
*/

#include "struct_init.h"
#include "pio.h"

#ifndef HASHMAP_H
#define HASHMAP_H
#define HASH_TABLE_SIZE (LEDMAT_COLS_NUM * 2 + 1)
//This functions hashes the given coordinates
uint8_t hash(uint8_t x, uint8_t y);

//Add sandbags to hash table
void hash_add(SandBag sandbag);

//Check whether the hash table contains a sandbag at a given coordinate.
//Returns dud sandbag if none found
SandBag hash_contains(uint8_t x, uint8_t y);
#endif

