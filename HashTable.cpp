/* simple fixed size hashtable with no chaining for use on constrained devices */

#include <string>
using namespace std;  // for memset

#include "HashTable.h"

HashTable::HashTable()
{
    this->entries = 0;
    memset(&table[0], sizeof(HashEntry) * HASH_TABLE_SIZE, 0);
}

float HashTable::usage()
{
    // return percentage table is filled
    return 100.0 * entries / (1.0 * HASH_TABLE_SIZE);
}

unsigned int HashTable::hash(String key)
{
    // Jenkins One-at-a-Time hash
    unsigned char *p = (unsigned char *)key;
    unsigned h = 0;
    int c;

    while ((c = *p++))
    {
        h += c;
        h += ( h << 10 );
        h ^= ( h >> 6 );
    }

    h += ( h << 3 );
    h ^= ( h >> 11 );
    h += ( h << 15 );

    return h;
}

int HashTable::strcmp(String s1, String s2)
{
    while(*s1 && (*s1==*s2))
        s1++,s2++;
    return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

void * HashTable::insert_key(String key, void *value)
{
    unsigned int i = 0, hashval = hash(key), index = hashval % HASH_TABLE_SIZE;
    HashEntry *entry;
    
    while (i++ < HASH_TABLE_SIZE &&  (entry = table+index, entry->key))
    {
        if (!strcmp(entry->key, key))
        {
            entry->value = value;
            return entry;
        }
            
        index = (++index % HASH_TABLE_SIZE);
    }
    
    if (i < HASH_TABLE_SIZE && entry)
    {
        entry->key = key;
        entry->value = value;
        ++entries;
        return entry;
    }
    
    return 0;
}

void * HashTable::find_key(String key)
{
    unsigned int i = 0, hashval = hash(key), index = hashval % HASH_TABLE_SIZE;
    HashEntry *entry;
    
    while (i++ < HASH_TABLE_SIZE &&  (entry = table+index, entry))
    {
        if (strcmp(entry->key, key))
            return entry;
            
        index = (++index % HASH_TABLE_SIZE);
    }
    
    return 0;
}
