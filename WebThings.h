// Web of Things Framework for Arduino IDE

#ifndef _WOTF_FRAMEWORK
#define _WOTF_FRAMEWORK

#include "WebCore.h"

class Thing : public CoreThing
{
    public:

        void set_start(ThingFunc handler);
        void set_stop(ThingFunc handler);
        void register_observer(Symbol event, EventFunc handler);
        void set_property(Symbol property, JSON *value);
        JSON *get_property(Symbol property);
        void invoke(Symbol action, ...);
};

class Proxy : public CoreThing
{
    public:

        void register_observer(Symbol event, EventFunc handler);
        void set_property(Symbol property, JSON *value);
        JSON *get_property(Symbol property);
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
