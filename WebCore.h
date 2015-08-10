// Web of Things Core class for Arduino IDE

#ifndef _WOTF_CORE
#define _WOTF_CORE

// forward references

class Thing; 
class JSON;

typedef void *Any;
typedef void (*SetupFunc)(Thing *thing, Names *names);
typedef void (*ThingFunc)(Thing *thing);
typedef void (*EventFunc)(Symbol event, Thing *thing, Any data);

// abstract class with methods common to Thing and Proxy
class CoreThing
{
    public:
        char *uri;  // URI for a thing or proxy
        Symbol id;  // identifies this thing in messages
        NPIndex model;  // parsed JSON-LD model
        NPIndex events;  // observers for each event
        NPIndex properties; // property values in JSON
        NPIndex actions; // functions implementing each action
        NPIndex proxies; // set of registered proxies for this thing
        CoreThing *next;  // linked list of registered things/proxies
    
        virtual void register_observer(Symbol event, EventFunc handler) = 0;
        virtual void register_observer(Names *names, unsigned char *event, EventFunc handler) = 0;
        virtual void set_property(Symbol property, JSON *value) = 0;
        virtual void set_property(Names *names, unsigned char *name, JSON *value) = 0;
        virtual JSON *get_property(Symbol property) = 0;
        virtual JSON *get_property(Names *names, unsigned char *name) = 0;
        virtual Thing *get_thing(Symbol property) = 0;
        virtual Thing *get_thing(Names *names, unsigned char *name) = 0;
        virtual void invoke(Symbol action, ...) = 0;
};

#endif
