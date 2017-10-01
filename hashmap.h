/* Description: Header file for the hash table holding sandbags.
 *              This allows for faster O(1) collision checking instead of
 *              O(n).
 * Author: George Khella
 * Date: October 2017
*/


static int hash_table_size = NUM_COLS * TRENCH_DEPTH * 2; //Double so load_factor = 0.5
static SandBag hash_table[hash_table_size];

/*The most simplistic version of a hash function...
  Takes either a sandbag or player if player is null, we hash sandbag
  and vice versa. */
int hash(int coord)

//Add item to hash table using Open Addressing with Linear Probing
//Only need sandbags in hashtable so player parameter non-existent
void hash_add(SandBag sandbag)

//Check whether the hash table contains a sandbag at current players coord.
//Returns NULL if no sandbag at player's coord
SandBag hash_contains(Player player, Bullet bullet)

