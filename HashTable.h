
#include "core.h"

// pick the table size based upon practical experience
// call usage() method to measure how full the table is
#define HASH_TABLE_SIZE  77

class HashTable
{
    public:
        HashTable();
        void * find_key(String key);
        void * insert_key(String key, void *value);
        float used();
            
    private:                
        class HashEntry 
        {
            public:
            
            String key;
            void * value;
        };
        
        unsigned int entries;
        HashEntry table[HASH_TABLE_SIZE];
        unsigned int hash(String key);
        int strcmp(String s1, String s2);
};

