typedef unsigned char *String;

// pick the table size based upon practical experience
// call usage() method to measure how full the table is
#define HASH_TABLE_SIZE  77

class HashTable
{
    public:
        HashTable();
        void * find_key(String key);
        void * insert_key(String key, void *value);
        float usage();
            
    private:
        unsigned int entries;
                
        static class HashEntry 
        {
            public:
            
            String key;
            void * value;
        } table[HASH_TABLE_SIZE];
        
        static unsigned int hash(String key);
        static int strcmp(String s1, String s2);
};

