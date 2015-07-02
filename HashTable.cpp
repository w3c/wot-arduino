/* simple fixed size hashtable with no chaining for use on constrained devices */

#include "HashTable.h"

HashTable::HashTable()
{
    this->entries = 0;
    memset(&table[0], 0, sizeof(HashEntry) * HASH_TABLE_SIZE);    
}

float HashTable::used()
{
    // return percentage table is filled
    return 100.0 * entries / (1.0 * HASH_TABLE_SIZE);
}

unsigned int HashTable::hash(unsigned char *key, unsigned int length)
{
    // Jenkins One-at-a-Time hash
    unsigned char *p = (unsigned char *)key;
    unsigned h = 0;

    while (length--)
    {
        h += *p++;
        h += ( h << 10 );
        h ^= ( h >> 6 );
    }

    h += ( h << 3 );
    h ^= ( h >> 11 );
    h += ( h << 15 );

    return h;
}

int HashTable::strcmp(unsigned char *s1, unsigned int len1, unsigned char *s2, unsigned int len2)
{
        
    if (len1 && len2)
    {
        while(len1-- && len2-- && (*s1==*s2))
        {
            s1++;
            s2++;
        }
    
        return *(const unsigned char*)s1-*(const unsigned char*)s2;
    }
        
    if (!(len1 | len2))
        return 0;

    return (len1 ? 1 : -1);
}

boolean HashTable::insert_key(unsigned char *key, unsigned int length, unsigned int value)
{
    unsigned int i = 0, hashval = hash(key, length), index = hashval % HASH_TABLE_SIZE;
    HashEntry *entry = 0;
    
    while (i++ < HASH_TABLE_SIZE && (entry = table+index, entry->key))
    {
        if (!strcmp(entry->key, entry->length, key, length))
        {
            entry->value = value;
            return true;
        }
            
        index = (++index % HASH_TABLE_SIZE);
    }
    
    if (i < HASH_TABLE_SIZE && entry)
    {
        entry->key = key;
        entry->length = length;
        entry->value = value;
        ++entries;
        return true;
    }
    
    return false;
}

unsigned int HashTable::find_key(unsigned char *key, unsigned int length)
{
    unsigned int i = 0, hashval = hash(key, length), index = hashval % HASH_TABLE_SIZE;
    HashEntry *entry;
    
    while (i++ < HASH_TABLE_SIZE &&  (entry = table+index, entry))
    {
        if (!strcmp(entry->key, entry->length, key, length))
            return entry->value;
            
        index = (++index % HASH_TABLE_SIZE);
    }
    
    return 0;
}
