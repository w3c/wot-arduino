// Web of Things Framework for Arduino IDE

#ifndef _WOTF_FRAMEWORK
#define _WOTF_FRAMEWORK

#include "WebCore.h"

class Thing : public CoreThing
{
    public:

        void set_start(ThingHandler handler);
        void set_stop(ThingHandler handler);
        void register_observer(const char *name, EventHandler handler);
        void unregister_observer(const char *name, EventHandler handler);
        void set_property(const char *name, Any value);
        Any get_property(const char *name);
        void invoke(const char *name, ...);
};

class Proxy : public CoreThing
{
    public:

        void register_observer(const char *name, EventHandler handler);
        void unregister_observer(const char *name, EventHandler handler);
        void set_property(const char *name, Any value);
        Any get_property(const char *name);
        void invoke(const char *name, ...);
};

class WebThings
{
    public:
        WebThings();
        Thing *thing(const char *name, const char *model);
        void register_proxy(const char *uri, ThingHandler succeed, ErrorHandler fail);
};

#endif
