#ifndef _WOT_REGISTRY
#define _WOT_REGISTRY

typedef uint8_t RIndex;

// Registry of local things and proxies for remote things
// URI strings can be held in different places

enum String_Tag { RAM_t, Flash_t, EEPROM_t };

class Registry
{
    public:
        static void initialise_pool(WotNodePool *pool);
        static Thing *find_thing(const char *uri, uint16_t length, String_Tag tag);
        static void register_thing(const char *uri, uint16_t length, String_Tag tag, Thing *thing);
        
        static Proxy *find_proxy(const char *uri, uint16_t length, String_Tag tag);
        static void register_proxy(const char *uri, uint16_t length, String_Tag tag, Proxy *proxy);
        
    private:
        class RNode
        {
            public:
                
                RIndex index;    // for a thing or proxy
                RIndex next;     // next entry in linked list
                uint16_t taglen; // 2 bit tag and & 14 bit length
                const char *uri; // pointer URI string

                int match(const char *uri, uint16_t length, String_Tag tag);
                String_Tag get_tag();
                void set_tag(String_Tag tag);
                void set_str_length(unsigned int length);
                unsigned int get_str_length();
        };
};

#endif