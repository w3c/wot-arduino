// Web of Things Core class for Arduino IDE

#ifndef _WOTF_CORE
#define _WOTF_CORE

class Thing; // forward reference

typedef uint8_t Symbol;  // used in place of names to save memory & message size

typedef void *Any;
typedef void (*ThingHandler)(Thing *thing);
typedef void (*EventHandler)(Symbol event, Thing *thing, Any data);

// abstract class with methods common to Thing and Proxy
class CoreThing
{
    public:
    
        virtual void register_observer(Symbol event, EventHandler handler) = 0;
        virtual void unregister_observer(Symbol event, EventHandler handler) = 0;
        virtual void set_property(Symbol property, Any value) = 0;
        virtual Any get_property(Symbol property) = 0;
        virtual void invoke(Symbol action, ...) = 0;
        virtual Symbol get_symbol(const char *name) = 0;
};

#endif
