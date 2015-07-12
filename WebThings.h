// Web of Things Framework for Arduino IDE

#ifndef _WOTF_FRAMEWORK
#define _WOTF_FRAMEWORK

#include "WebCore.h"

class Thing : public CoreThing
{
    public:

        void set_start(ThingHandler handler);
        void set_stop(ThingHandler handler);
        void register_observer(Symbol event, EventHandler handler);
        void unregister_observer(Symbol event, EventHandler handler);
        void set_property(Symbol property, Any value);
        Any get_property(Symbol property);
        void invoke(Symbol action, ...);
        Symbol get_symbol(const char *name);

};

class Proxy : public CoreThing
{
    public:

        void register_observer(Symbol event, EventHandler handler);
        void unregister_observer(Symbol event, EventHandler handler);
        void set_property(Symbol property, Any value);
        Any get_property(Symbol property);
        void invoke(Symbol action, ...);
        Symbol get_symbol(const char *name);
};

class WebThings
{
    public:
        WebThings();
        float used();
        void thing(const char *name, const char *model, ThingHandler setup);
        void register_proxy(const char *uri, ThingHandler setup);
};

#endif
