// hash table for symbol dictionary

#ifndef _WOTF_HASHTABLE
#define _WOTF_HASHTABLE

#include "core.h"

// pick the table size based upon practical experience
// call usage() method to measure how full the table is

#define HASH_TABLE_SIZE  77

class HashTable
{
    public:
        HashTable();
        unsigned int find_key(unsigned char *key, unsigned int length);
        boolean insert_key(unsigned char *key, unsigned int length, unsigned int value);
        float used();
            
    private:                
        class HashEntry 
        {
            public:
            
            unsigned char *key;
            unsigned int length;
            unsigned int value;
        };
        
        unsigned int entries;
        HashEntry table[HASH_TABLE_SIZE];
        unsigned int hash(unsigned char *key, unsigned int length);
        int strcmp(unsigned char *s1, unsigned int len1, unsigned char *s2, unsigned int len2);
};

#endif