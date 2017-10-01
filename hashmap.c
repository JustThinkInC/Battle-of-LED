/* Description: Code file for hash map used by collision checker.
 *              Every sandbag is hashed into a hash table. Collision
 *              between hashes is resolved by Open Addressing with Linear
 *              Probing.
 * Author: George Khella
 * Date: October 2017
*/

/*The most simplistic version of a hash function...
  Takes either a sandbag or player if player is null, we hash sandbag
  and vice versa. */
int hash(int coord)
{
    int number = coord % hash_table_size;

    return number;
}

//Add item to hash table using Open Addressing with Linear Probing
//Only need sandbags in hashtable so player parameter non-existent
void hash_add(SandBag sandbag)
{
    int hash_slot = hash(sandbag);

    if (!hash_table[hash_slot]) {
        hash_table[hash_slot] = sandbag;
    } else {
        while(hash_table[hash_slot]) {
            hash_slot ++;
            hash_slot >= hash_table_size ? hash_slot = 0 : 0;
        }
        hash_table[hash_slot] = sandbag;
    }

}

//Check whether the hash table contains a sandbag at current players coord.
//Returns NULL if no sandbag at player's coord
SandBag hash_contains(Player player, Bullet bullet)
{
    if (player) {
        Player item = player; 
    } else {
        Bullet item = bullet;
    }
    int first_hash = hash(item.x+item.y);
    have_wrapped = 0;
    SandBag sandbag = hash_table[first_hash];

    if (sandbag.x == item.x && sandbag.y == item.y) {
        return sandbag;
    } else {
        int current_index = first_hash;

        while (hash_table[current_index] != NULL) {
            sandbag = hash_table[current_index];
            if (sandbag.x == item.x && sandbag.y == item.y) {
                return sandbag;
            }
            if ((current_index == first_hash) && have_wrapped) {
                //Item not found, hashtable full
                return NULL;
            }
            if (current_index == (hash_table_size-1)) {
                //Wrap back to start of hash table
                current_index = 0;
                have_wrapped = 1;
            } else {
                current_index += 1;
            }

            return NULL;
        }
    }
}
