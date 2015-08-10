// hash table for symbol dictionary

#ifndef _WOTF_NAMES
#define _WOTF_NAMES

#ifndef null
#define null 0
#endif

// pick the table size based upon practical experience
// call usage() method to measure how full the table is

#define HASH_TABLE_SIZE  31

typedef uint8_t Symbol;  // used in place of names to save memory & message size

class Names
{
    public:
        Names();
        unsigned int find_key(const unsigned char *key, unsigned int length);
        boolean insert_key(const unsigned char *key, unsigned int value);
        boolean insert_key(const unsigned char *key, unsigned int length, unsigned int value);
        Symbol get_symbol(const unsigned char *key, unsigned int length, unsigned int *count);
        Symbol get_symbol(const unsigned char *key);
        
#ifdef DEBUG
        void print();
#endif
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
        int strlen(const unsigned char *str);
        int strcmp(const unsigned char *s1, unsigned int len1, const unsigned char *s2, unsigned int len2);
};

#endif