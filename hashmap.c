/* Description: Code file for hash map used by collision checker.
 *              Every sandbag is hashed into a hash table.
 * Author: George Khella
 * Date: October 2017
*/


#include "hashmap.h"
#include "struct_init.h"
#include "pio.h"
#include "stdlib.h"

#define HASH_TABLE_SIZE 11//LEDMAT_COLS_NUM*2+1
SandBag hash_table[HASH_TABLE_SIZE];


/*Hash function that takes an x and y coord multiplies each one by an 
 arbitrarily chosen number then computes the result modulo hash table size*/
uint8_t hash(uint8_t x, uint8_t y)
{
    //This formula gives different hashes for our scenario,
    uint8_t number = ((x*3 + y*5) << 3) % HASH_TABLE_SIZE;

    return number;
}

//Add sandbag to hashtable, note that no other game objects may be stored
void hash_add(SandBag sandbag)
{
    uint8_t hash_slot = hash(sandbag.pos.x, sandbag.pos.y);
    hash_table[hash_slot] = sandbag;
}

//Check whether the hash table contains a sandbag at a given game object's coord.
//Returns 'dud' sandbag if no sandbag at player's coord
SandBag hash_contains(uint8_t x, uint8_t y)
{
    uint8_t hash_slot = hash(x, y);
    SandBag sandbag = hash_table[hash_slot];
    SandBag dud;
    dud.pos.x = 0;
    dud.pos.y = 0;
    dud.health = 0;

    if (sandbag.pos.x == x && sandbag.pos.y == y) {
        return sandbag;
    }

    return dud;
}
