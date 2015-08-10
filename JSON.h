// Minimal JSON support for Arduino

#ifndef _WOTF_JSON
#define _WOTF_JSON

#ifndef null
#define null 0
#endif

// Function and Proxy are special nodes that are used for things
enum Json_Tag { Unused_t, // reserved for unused JSON nodes
        Object_t, Array_t, String_t, Unsigned_t,
        Signed_t, Float_t, Boolean_t, Null_t,
        Function_t, Proxy_t, Thing_t };

enum Json_Token { Error_token, String_token, Colon_token, Comma_token,
        Object_start_token, Object_stop_token, Array_start_token, Array_stop_token,
        Float_token, Unsigned_token, Signed_token, Null_token, True_token, False_token};

// forward references
class Thing;
class Proxy;
class JSON; 

typedef void (*GenericFn)(JSON *data);

#define JSON_SYMBOL_BASE 10

// JSON is 6 bytes on ATmega328 and 32 bit MCUs, and 10 bytes on 64 bit computers

class JSON
{
    public:
        static void initialise_pool(WotNodePool *wot_node_pool);
        static JSON * parse(const char *src, unsigned int length, Names *names);
        static JSON * parse(const char *src, Names *names);
        
#ifdef DEBUG
        static void print_string(const unsigned char *name, unsigned int length);
        static void print_name_value(AvlKey key, AvlValue value, void *context);
        static void print_array_item(AvlKey key, AvlValue value, void *context);
#endif

        static JSON * new_unsigned(unsigned int x);
        static JSON * new_signed(int x);
        static JSON * new_float(float x);
        static JSON * new_null();
        static JSON * new_boolean(boolean value);
        static JSON * new_string(unsigned char *str, unsigned int length);
        static JSON * new_object();
        static JSON * new_array();
        static JSON * new_function(GenericFn func);

        void print();
        Json_Tag json_type();
        
        boolean is_null();
        boolean get_boolean();
        unsigned char *get_string(unsigned int *length);
        unsigned int get_unsigned();
        int get_signed();
        float get_float();
        Thing *get_thing();
        Proxy *get_proxy();
        
        void insert_property(unsigned int symbol, JSON *value);
        JSON * retrieve_property(unsigned int symbol);
        
        void append_array_item(JSON *value);
        void insert_array_item(unsigned int index, JSON *value);
        JSON * retrieve_array_item(unsigned int index);
        GenericFn retrieve_function(Symbol symbol);

    private:
        class Lexer
        {
            public:
                Names *names;
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
        
        // to reduce the size of JSON Nodes we combine the tag
        // and string token length into a uint16_t relying on
        // special getter and setter methods to hide this

        Json_Tag get_tag();
        void set_tag(Json_Tag tag);
        void set_str_length(unsigned int length);
        unsigned int get_str_length();
        
        uint16_t taglen;  // composite field for tag and string length
        
        union js_union
        {
            Thing *thing;
            Proxy *proxy;
            unsigned char *str;
            float number;
            unsigned int u;
            int i;
            boolean truth;
            AvlIndex object;
            GenericFn function;
        } variant;
};

#endif