// Web of Things Core class for Arduino IDE

#ifndef _WOTF_CORE
#define _WOTF_CORE

// forward references

class Thing; 
class JSON;

#define MAX_THINGS 8

typedef uint8_t CTIndex;

typedef void *Any;
typedef void (*SetupFunc)(Thing *thing, Names *table);
typedef void (*ThingFunc)(Thing *thing);
typedef void (*EventFunc)(Symbol event, Thing *thing, ...);
typedef void (*ActionFunc)(unsigned int proxy_id, unsigned int action_id, ...);
typedef void (*ResponseFunc)(unsigned int id, Thing *thing, ...);

// abstract class with methods common to Thing and Proxy
class CoreThing
{
    public:
        Symbol id;  // identifies this thing in messages
        char *uri;  // URI for a thing or proxy
        NPIndex model;  // parsed JSON-LD model
        NPIndex events;  // observers for each event
        NPIndex properties; // property values in JSON
        NPIndex actions; // functions implementing each action
        NPIndex proxies; // set of registered proxies for this thing
        CoreThing *next;  // linked list of registered things/proxies
    
        virtual void register_observer(Symbol event, EventFunc handler) = 0;
        virtual void raise_event(Symbol event, ...) = 0;
        virtual void set_property(Symbol property, JSON *value) = 0;
        virtual JSON *get_property(Symbol property) = 0;
        virtual int invoke(Symbol action, ...) = 0;
        virtual void reachable(boolean phase) = 0;
        virtual void sweep(boolean phase) = 0;
        virtual void remove() = 0;
};

// for managing allocation of thing and proxy objects from static pool
class ThingPool
{
    public:
        static CoreThing *allocate();
        static void free(CoreThing *thing);
        static unsigned int size();
        static float used();
};

#endif
