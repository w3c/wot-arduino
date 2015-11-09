// Web of Things Framework for Arduino IDE

#ifndef _WOTF_FRAMEWORK
#define _WOTF_FRAMEWORK

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


class Thing : public CoreThing
{
    public:

        void set_start(ThingFunc handler);
        void set_stop(ThingFunc handler);
        void register_observer(Symbol event, EventFunc handler);
        void register_observer(Names *names, unsigned char *event, EventFunc handler);
        void set_property(Symbol property, JSON *value);
        void set_property(Names *names, unsigned char *property, JSON *value);
        JSON *get_property(Symbol property);
        JSON *get_property(Names *names, unsigned char *property);
        Thing *get_thing(Symbol property);
        Thing *get_thing(Names *names, unsigned char *property);
        void invoke(Symbol action, ...);
};

class Proxy : public CoreThing
{
    public:

        void register_observer(Symbol event, EventFunc handler);
        void register_observer(Names *names, unsigned char *event, EventFunc handler);
        void set_property(Symbol property, JSON *value);
        void set_property(Names *names, unsigned char *property, JSON *value);
        JSON *get_property(Symbol property);
        JSON *get_property(Names *names, unsigned char *property);
        Thing *get_thing(Symbol property);
        Thing *get_thing(Names *names, unsigned char *property);
        void invoke(Symbol action, ...);
};

class WebThings
{
    public:
        WebThings();
        static float used();
        static void thing(const char *name, const char *model, SetupFunc setup);
        static void register_proxy(const char *uri, ThingFunc setup);
        static JSON *get_json(NPIndex index);
        static NPIndex get_index(JSON *json);
        
    private:
        static Thing *things;
        static Proxy *proxies;
        
        static Thing *find_thing(const char *name);
        static Proxy *find_proxy(const char *name);
        static void register_thing(Thing *thing);
        static boolean strcmp(const char *s1, const char *s2);
};

#endif
