// Web of Things Framework for Arduino IDE

#ifndef _WOTF_FRAMEWORK
#define _WOTF_FRAMEWORK

#include "WebCore.h"

#define MAX_STALE 10

class Thing : public CoreThing
{
    public:
        void set_start(ThingFunc handler);
        void set_stop(ThingFunc handler);
        void set_property(Symbol property, JSON *value);
        JSON *get_property(Symbol property);
        int invoke(Symbol action, ...);
        void register_action_handler(Symbol action, ActionFunc handler);
        void register_observer(Symbol event, EventFunc handler);
        void raise_event(Symbol event, ...);
        void reachable(boolean phase);
        void sweep(boolean phase);
        void remove();
        void print();
};

class Proxy : public CoreThing
{
    public:
        void set_property(Symbol property, JSON *value);
        JSON *get_property(Symbol property);
        int invoke(Symbol action, ...);
        void register_response_handler(Symbol action, ResponseFunc handler);
        void register_observer(Symbol event, EventFunc handler);
        void raise_event(Symbol event, ...);
        void reachable(boolean phase);
        void sweep(boolean phase);
        void remove();
        void print();
};

class WebThings
{
    public:
        WebThings();
        static unsigned int used();
#if defined(pgm_read_byte)
        static void thing(const char *name, const __FlashStringHelper *model, SetupFunc setup);
#endif
        static void thing(const char *name, char *model, SetupFunc setup);
        static void register_proxy(const char *uri, ThingFunc setup);
        static JSON *get_json(NPIndex index);
        static NPIndex get_index(JSON *json);
        static void collect_garbage();
        static void remove_thing(Thing *thing);
        static void remove_proxy(Proxy *proxy);
        static void add_stale(JSON *json);
        
    private:
        static void reachable(NPIndex index);
        static Thing *find_thing(const char *name);
        static Proxy *find_proxy(const char *name);
        static void register_thing(Thing *thing);
        static boolean strcmp(const char *s1, const char *s2);
};

#endif
