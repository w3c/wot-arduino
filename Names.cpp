/* simple fixed size hashtable with no chaining for use on constrained devices */

#include <Arduino.h>
#include "Names.h"

Names::Names()
{
    this->entries = 0;
    memset(&table[0], 0, sizeof(HashEntry) * HASH_TABLE_SIZE);    
}

float Names::used()
{
    // return percentage table is filled
    return 100.0 * entries / (1.0 * HASH_TABLE_SIZE);
}

#ifdef DEBUG

void Names::print()
{
    PRINT(F("Hash table has "));
    PRINT(entries);
    PRINT(F(" entries, "));
    PRINT(used());
    PRINTLN(F("% full\n"));
    
    for (int i = HASH_TABLE_SIZE; i > 0; )
    {
        HashEntry *entry = table + (--i);
        
        if (entry->key)
        {
            PRINT("  "); PRINT((const char *)entry->key); PRINT(" : "); PRINTLN(entry->value);
        }
    }

}

#endif

unsigned int Names::hash(const unsigned char *key, unsigned int length)
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

int Names::strlen(const unsigned char *str)
{
    int len = 0;
    
    while (*str++)
        ++len;
        
    return len;
}

int Names::strcmp(const unsigned char *s1, unsigned int len1,
                        const unsigned char *s2, unsigned int len2)
{  
    if (len1 && len2)
    {
        while(len1-- && len2-- && (*s1==*s2))
        {
            s1++;
            s2++;
        }
    
        return *s1-*s2;
    }
        
    if (!(len1 | len2))
        return 0;

    return (len1 ? 1 : -1);
}

Symbol Names::get_symbol(const unsigned char *key)
{
    unsigned int length = strlen(key);
    unsigned int i = HASH_TABLE_SIZE, hashval = hash(key, length), index = hashval % HASH_TABLE_SIZE;
    HashEntry *entry = 0;
    
    while (i-- && (entry = table+index, entry->key))
    {
        if (!strcmp(entry->key, entry->length, key, length))
        {
            return entry->value;
        }
            
        index = (++index % HASH_TABLE_SIZE);
    }
    
    return 0;
}

Symbol Names::get_symbol(const unsigned char *key, unsigned int length, unsigned int *count)
{
    unsigned int i = HASH_TABLE_SIZE, hashval = hash(key, length), index = hashval % HASH_TABLE_SIZE;
    HashEntry *entry = 0;
    
    while (i-- && (entry = table+index, entry->key))
    {
        if (!strcmp(entry->key, entry->length, key, length))
        {
            return entry->value;
        }
            
        index = (++index % HASH_TABLE_SIZE);
    }
    
    if (i && entry)
    {
        entry->key = key;
        entry->length = length;
        entry->value = *count;
        *count = *count + 1;
        ++entries;
        return entry->value;
    }
    
    return 0;
}

boolean Names::insert_key(const unsigned char *key, unsigned int value)
{
    unsigned int length = strlen(key);
    return insert_key(key, length, value);
}

boolean Names::insert_key(const unsigned char *key, unsigned int length, unsigned int value)
{
    unsigned int i = HASH_TABLE_SIZE, hashval = hash(key, length), index = hashval % HASH_TABLE_SIZE;
    HashEntry *entry = 0;
    
    while (i-- && (entry = table+index, entry->key))
    {
        if (!strcmp(entry->key, entry->length, key, length))
        {
            entry->value = value;
            return true;
        }
            
        index = (++index % HASH_TABLE_SIZE);
    }
    
    if (i && entry)
    {
        entry->key = key;
        entry->length = length;
        entry->value = value;
        ++entries;
        return true;
    }
    
    return false;
}

unsigned int Names::find_key(const unsigned char *key, unsigned int length)
{
    unsigned int i = HASH_TABLE_SIZE, hashval = hash(key, length), index = hashval % HASH_TABLE_SIZE;
    HashEntry *entry;
    
    while (i-- &&  (entry = table+index, entry))
    {
        if (!strcmp(entry->key, entry->length, key, length))
            return entry->value;
            
        index = (++index % HASH_TABLE_SIZE);
    }
    
    return 0;
}
