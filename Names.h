// hash table for symbol dictionary

#ifndef _WOTF_NAMES
#define _WOTF_NAMES

#ifndef null
#define null 0
#endif

// pick the table size based upon practical experience
// call usage() method to measure how full the table is

#define HASH_TABLE_SIZE  31

#if defined(pgm_read_byte)
#define PROGMEM_BOUNDARY 0x8000
#endif

class Names
{
    public:
        Names();
#if defined(pgm_read_byte)
        unsigned int symbol(const __FlashStringHelper *name);
#endif
        unsigned int symbol(const char *name);
        unsigned int symbol(const char *name, unsigned int length);
        void print();
        float used();
            
    private:                
        class HashEntry 
        {
            public:
            
            const char *name;
            unsigned int length;
            unsigned int symbol;
        };

        unsigned int entries;
        HashEntry table[HASH_TABLE_SIZE];
        unsigned int hash(const char *name, unsigned int length);
};

#endif