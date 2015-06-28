/* Web of Things Framework for Arduino IDE */

#include <string>
#include <stdarg.h>
#include "JSON.h"

class Thing; // forward reference

typedef void *Any;
typedef void (*ThingHandler)(Thing *thing);
typedef void (*ErrorHandler)(string error);
typedef void (*EventHandler)(string event, Thing *thing, Any data);

#define null 0
#define false 0
#define true  (!false)

// abstract class with methods common to Thing and Proxy
class CoreThing
{
    public:
    
        virtual void register_observer(string name, EventHandler handler) = 0;
        virtual void unregister_observer(string name, EventHandler handler) = 0;
        virtual void set_property(string name, Any value) = 0;
        virtual Any get_property(string name) = 0;
        virtual void invoke(string name, ...) = 0;
};

class Thing : public CoreThing
{
    public:

        void set_start(ThingHandler handler);
        void set_stop(ThingHandler handler);
        void register_observer(string name, EventHandler handler);
        void unregister_observer(string name, EventHandler handler);
        void set_property(string name, Any value);
        Any get_property(string name);
        void invoke(string name, ...);
};

class Proxy : public CoreThing
{
    public:

        void register_observer(string name, EventHandler handler);
        void unregister_observer(string name, EventHandler handler);
        void set_property(string name, Any value);
        Any get_property(string name);
        void invoke(string name, ...);
};

class WebThings
{
    public:
        Thing *thing(string name, string model);
        void register_proxy(string uri, ThingHandler succeed, ErrorHandler fail);
        
    private:
        // memory pools for JSON, Strings, AVLTrees
        // to be added later
};