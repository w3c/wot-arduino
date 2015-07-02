/* Minimal JSON support for Arduino */

#include "core.h"
#include "AvlNode.h"
#include "HashTable.h"

using namespace std;  // for string and memset

enum Json_Tag { Object_t, String_t, Unsigned_t, Signed_t, Float_t, Boolean_t, Null_t };

class JSON
{
    public:
        static void initialise_pool(JSON *pool, unsigned int size);
        static JSON * parse(unsigned char *src, unsigned int length);

        static JSON * new_unsigned(unsigned int x);
        static JSON * new_signed(int x);
        static JSON * new_float(float x);
        static JSON * new_null();
        static JSON * new_boolean(boolean value);
        static JSON * new_string(unsigned char *str);
        static JSON * new_object();

        void insert(unsigned int symbol, JSON *value);
        JSON * retrieve(unsigned int symbol);
        Json_Tag json_type();
        
    private:
        class JSONLexer
        {
            public:
                HashTable table;
                unsigned char *src;
                unsigned int length;
        };
        
        static unsigned int length;
        static unsigned int size;
        static JSON *pool;
        
        static JSON * new_node();
        static JSON * parse_private(JSONLexer *lexer);
        
        Json_Tag tag;
        
        union js_union
        {
            unsigned char *str;
            float number;
            unsigned int u;
            int i;
            boolean truth;
            JSON *array;
            AvlNode *object;
        } variant;
};
