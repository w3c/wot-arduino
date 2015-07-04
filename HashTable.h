// hash table for symbol dictionary

#ifndef _WOTF_HASHTABLE
#define _WOTF_HASHTABLE

// pick the table size based upon practical experience
// call usage() method to measure how full the table is

#define HASH_TABLE_SIZE  77

class HashTable
{
    public:
        HashTable();
        unsigned int find_key(const unsigned char *key, unsigned int length);
        boolean insert_key(const unsigned char *key, unsigned int value);
        boolean insert_key(const unsigned char *key, unsigned int length, unsigned int value);
        unsigned int get_symbol(const unsigned char *key, unsigned int length, unsigned int *count);
        void print();
        float used();
            
    private:                
        class HashEntry 
        {
            public:
            
            const unsigned char *key;
            unsigned int length;
            unsigned int value;
        };
        
        unsigned int entries;
        HashEntry table[HASH_TABLE_SIZE];
        unsigned int hash(const unsigned char *key, unsigned int length);
        int strcmp(const unsigned char *s1, unsigned int len1, const unsigned char *s2, unsigned int len2);
};

#endif