// Web of Things Framework for Arduino IDE

#ifndef _WOTF_FRAMEWORK
#define _WOTF_FRAMEWORK

#include <stdarg.h>
#include "JSON.h"

class Thing; // forward reference

typedef void *Any;
typedef void (*ThingHandler)(Thing *thing);
typedef void (*ErrorHandler)(unsigned char *error);
typedef void (*EventHandler)(unsigned char *event, Thing *thing, Any data);

// size of the various memory pools

#define AVL_NODE_POOL_SIZE 80
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
