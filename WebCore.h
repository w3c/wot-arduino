// Web of Things Core class for Arduino IDE

#ifndef _WOTF_CORE
#define _WOTF_CORE

class Thing; // forward reference

typedef void *Any;
typedef void (*ThingHandler)(Thing *thing);
typedef void (*ErrorHandler)(unsigned char *error);
typedef void (*EventHandler)(unsigned char *event, Thing *thing, Any data);

// size of the various memory pools

#define AVL_NODE_POOL_SIZE 40
#define JSON_NODE_POOL_SIZE 30

// abstract class with methods common to Thing and Proxy
class CoreThing
{
    public:
    
        virtual void register_observer(const char *name, EventHandler handler) = 0;
        virtual void unregister_observer(const char *name, EventHandler handler) = 0;
        virtual void set_property(const char *name, Any value) = 0;
        virtual Any get_property(const char *name) = 0;
        virtual void invoke(const char *name, ...) = 0;
};

#endif
