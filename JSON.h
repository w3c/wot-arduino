// Minimal JSON support for Arduino

#ifndef _WOTF_JSON
#define _WOTF_JSON

#ifndef null
#define null 0
#endif


// Function and Proxy are special nodes that are used for things
enum Json_Tag { Unused_t, // used to identify unused JSON nodes
        Object_t, Array_t, String_t, Unsigned_t,
        Signed_t, Float_t, Boolean_t, Null_t,
        Function_t, Proxy_t, Thing_t };

enum Json_Token {Error_token, String_token, Colon_token, Comma_token,
        Object_start_token, Object_stop_token, Array_start_token, Array_stop_token,
        Float_token, Unsigned_token, Signed_token, Null_token, True_token, False_token};
        
class JSON; // forward reference

typedef void (*GenericFn)(JSON *data);
typedef uint8_t Symbol;  // used in place of names to save memory & message size

#define JSON_SYMBOL_BASE 10

// forward references
class Thing;
class Proxy;

class JSON
{
    public:
        static void initialise_pool(WotNodePool *wot_node_pool);
#if defined(pgm_read_byte)
        static JSON * parse(const __FlashStringHelper *, Names *table);
#endif
        static JSON * parse(const char *, Names *table);
        static void print_string(const char *name, unsigned int length);
        static void print_name_value(AvlKey key, AvlValue value, void *context);
        static void print_array_item(AvlKey key, AvlValue value, void *context);
        static void reachable_item(AvlKey key, AvlValue value, void *context);
        static void sweep_item(AvlKey key, AvlValue value, void *context);
        static JSON * new_unsigned(unsigned int x);
        static JSON * new_signed(int x);
        static JSON * new_float(float x);
        static JSON * new_null();
        static JSON * new_boolean(boolean value);
        static JSON * new_string(const __FlashStringHelper *str);
        static JSON * new_string(char *str);
        static JSON * new_string(char *str, unsigned int length);
        static JSON * new_object();
        static JSON * new_array();
        static JSON * new_function(GenericFn func);
        static void set_gc_phase(boolean phase);
        
        boolean free_leaves();
        void reachable(boolean phase);
        void sweep(boolean phase);
        boolean marked(boolean phase);

        void print();
        Json_Tag get_tag();
        
        void insert_property(Symbol symbol, JSON *value);
        JSON * retrieve_property(Symbol symbol);
        
        GenericFn retrieve_function(Symbol symbol);

        void append_array_item(JSON *value);
        void insert_array_item(unsigned int index, JSON *value);
        JSON * retrieve_array_item(unsigned int index);
        
    private:
        class Lexer
        {
            public:
                Names *table;
                const char  *src;
                unsigned int length;
                char *token_src;
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
        static JSON * parse(const char *, unsigned int length, Names *table);
        
        // the first 2 bytes combine the 4 bit JSON tag, a mark/sweep
        // flag and 11 bits for the string length or object id

        void set_tag(Json_Tag tag);
        void set_obj_id(unsigned int id);
        unsigned int get_obj_id();
        void set_str_length(unsigned int length);
        unsigned int get_str_length();
        void toggle_mark();
        void set_mark();
        void reset_mark();
        void check_if_stale(JSON * old_value, JSON * new_value);
        void free();

        uint16_t taglen;  // composite field for tag, mark, id and string length
        
        union js_union
        {
            char *str; // 16 bits on AVR
            float number;  // 32 bits on AVR
            unsigned int u;
            int i;   // 16 bits on AVR
            boolean truth;
            AvlIndex object;
            Thing *thing;
            Proxy *proxy;
            GenericFn function; // 16 bits on AVR
        } variant;
};

#endif