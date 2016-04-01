/* simple fixed size hashtable with no chaining for use on constrained devices */

#include <Arduino.h>
#include "Strings.h"
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

void Names::print()
{
    Serial.print(F("Hash table has "));
    Serial.print(entries);
    Serial.print(F(" entries, "));
    Serial.print(used());
    Serial.println(F("% full"));
    
    for (int i = HASH_TABLE_SIZE; i > 0; )
    {
        HashEntry *entry = table + (--i);
        
        if (entry->name)
        {
            char *p = (char *)entry->name;
            Serial.print("  ");
            
            for (int j = entry->length; j; --j)
                Serial.print(Strings::get_char(p++));
                          
            Serial.print(" : ");
            Serial.println(entry->symbol);
        }
    }

}

unsigned int Names::hash(const char *name, unsigned int length)
{
    // Jenkins One-at-a-Time hash
    unsigned h = 0;

    while (length--)
    {
        h += (unsigned char)Strings::get_char(name++);
        h += ( h << 10 );
        h ^= ( h >> 6 );
    }

    h += ( h << 3 );
    h ^= ( h >> 11 );
    h += ( h << 15 );

    return h;
}

#if defined(pgm_read_byte)
unsigned int Names::symbol(const __FlashStringHelper *name)
{
    return symbol(((const char *)name)+PROGMEM_BOUNDARY);
}
#endif

unsigned int Names::symbol(const char *name)
{
    return symbol(name, Strings::strlen(name));
}

unsigned int Names::symbol(const char *name, unsigned int length)
{
    unsigned int i = HASH_TABLE_SIZE;
    unsigned int hashval = hash(name, length);
    unsigned int index = hashval % HASH_TABLE_SIZE;
    HashEntry *entry = 0;
    
    while (i-- && (entry = table+index, entry->name))
    {
        // symbol already defined
        if (!Strings::strcmp(entry->name, entry->length, name, length))
            return entry->symbol;
            
        index = ++index % HASH_TABLE_SIZE;
    }
    
    // need to define new symbol
    if (i && entry)
    {
        entry->name = name;
        entry->length = length;
        entry->symbol = entries++;
        return entry->symbol;
    }
    
    return 0;
}
