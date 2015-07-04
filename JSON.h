// Minimal JSON support for Arduino

#ifndef _WOTF_JSON
#define _WOTF_JSON

#include "core.h"
#include "AvlNode.h"
#include "HashTable.h"

using namespace std;  // for string and memset

enum Json_Tag { Object_t, Array_t, String_t, Unsigned_t, Signed_t, Float_t, Boolean_t, Null_t };

enum Json_Token {Error_token, String_token, Colon_token, Comma_token,
        Object_start_token, Object_stop_token, Array_start_token, Array_stop_token,
        Float_token, Unsigned_token, Signed_token, Null_token, True_token, False_token};

class JSON
{
    public:
        static void initialise_json_pool(JSON *pool, unsigned int size);
        static float json_pool_used();
        static JSON * parse(const char *src, unsigned int length);
        static JSON * parse(const char *src);
        static void print_string(const unsigned char *name, unsigned int length);
        static void print_name_value(AvlKey key, AvlValue value, void *context);
        static void print_array_item(AvlKey key, AvlValue value, void *context);

        static JSON * new_unsigned(unsigned int x);
        static JSON * new_signed(int x);
        static JSON * new_float(float x);
        static JSON * new_null();
        static JSON * new_boolean(boolean value);
        static JSON * new_string(unsigned char *str, unsigned int length);
        static JSON * new_object();
        static JSON * new_array();

        void print();
        Json_Tag json_type();
        
        void insert_property(unsigned int symbol, JSON *value);
        JSON * retrieve_property(unsigned int symbol);
        
        void insert_array_item(unsigned int symbol, JSON *value);
        JSON * retrieve_array_item(unsigned int symbol);

    private:
        class Lexer
        {
            public:
                HashTable table;
                unsigned int symcount;
                unsigned char *src;
                unsigned int length;
                unsigned char *token_src;
                unsigned int token_len;
                unsigned int unsigned_num;
                int signed_num;
                float float_num;
                Json_Token get_token();
                Json_Token get_number(unsigned int c);
                Json_Token get_string();
                Json_Token get_special(unsigned int c);
                boolean end_of_array();
                void next_byte();
                unsigned int peek_byte();
        };
        
        static unsigned int json_pool_length;
        static unsigned int json_pool_size;
        static JSON * json_pool;
        
        static JSON * new_node();
        static JSON * parse_private(Lexer *lexer);
        static JSON * parse_object(Lexer *lexer);
        static JSON * parse_array(Lexer *lexer);
        
        // to reduce the size of JSON Nodes we could get creative &
        // combine the tag and string token length into a uint16_t
        // using special getter and setter methods to hide this
        Json_Tag tag;
        unsigned int token_len; // used for string lengths
        
        union js_union
        {
            unsigned char *str;
            float number;
            unsigned int u;
            int i;
            boolean truth;
            AvlNode *object;
        } variant;
};

#endif