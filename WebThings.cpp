/* Web of Things Framework for Arduino */

#include <stdarg.h>
#include <Arduino.h>
#include "NodePool.h"
#include "AvlNode.h"
#include "Names.h"
#include "JSON.h"
#include "WebThings.h"

#ifndef null
#define null 0
#endif

WotNodePool wot_node_pool;
Thing *WebThings::things;
Proxy *WebThings::proxies;

WebThings::WebThings()
{
    AvlNode::initialise_pool(&wot_node_pool);
    JSON::initialise_pool(&wot_node_pool);
    
#ifdef DEBUG
    PRINT(F("avl node size: "));
    PRINT((sizeof(AvlNode)));
    PRINTLN(F(" bytes"));
    
    PRINT(F("json node size: "));
    PRINT((sizeof(JSON)));
    PRINTLN(F(" bytes"));
    
    PRINT(F("pool node size: "));
    PRINT((sizeof(wot_node_pool_t)));
    PRINTLN(F(" bytes"));
    
    PRINT(F("WoT pool size: "));
    PRINT(wot_node_pool.size());
    PRINTLN(F(" bytes"));
#endif
}

float WebThings::used()
{
    return wot_node_pool.used();
}

void WebThings::thing(const char *name, const char *model, SetupFunc setup)
{
    Names names;
    unsigned int id = 0;
    Thing *t = things, *thing = new Thing();
    
    if (thing)
    {
        while (t)
        {
            ++id;
            t = (Thing *)t->next;
        }
    
        thing->uri = (char *)name;
        thing->id = ++id;
        thing->model = get_index(JSON::parse(model, &names));
        thing->events = get_index(JSON::new_object());
        thing->properties = get_index(JSON::new_object());
        thing->actions = get_index(JSON::new_object());
        thing->proxies = get_index(JSON::new_array());
    
        thing->next = (Thing *)things;
        things = thing;
        setup(thing, &names);
    }
}

// find the proxy's data model and set up new proxy object
// *** to be implemented ***
void WebThings::register_proxy(const char *uri, ThingFunc setup)
{
    Proxy *proxy = find_proxy(uri);
    
    if (!proxy)
    {
        proxy = new Proxy();
    }
}

Thing *WebThings::find_thing(const char *name)
{
    Thing *thing = things;
    
    while (thing)
    {
        if (!strcmp(thing->uri, name))
            return thing;
    }
    
    return thing;
}

Proxy *WebThings::find_proxy(const char *name)
{
    Proxy *proxy = proxies;
    
    while (proxy)
    {
        if (!strcmp(proxy->uri, name))
            return proxy;
    }
    
    return proxy;
}

void WebThings::register_thing(Thing *thing)
{
    thing->next = things;
    things = thing;
}

boolean WebThings::strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s2 && *s1++ == *s2++);
    return (boolean)(*s1 == *s1);
}

JSON *WebThings::get_json(NPIndex index)
{
    wot_node_pool_t *pool = wot_node_pool.get_pool();
    return (JSON *)(pool + index - 1);
}

NPIndex WebThings::get_index(JSON *json)
{
    if (json)
    {
        wot_node_pool_t *pool = wot_node_pool.get_pool();
        return (NPIndex)((wot_node_pool_t *)json - pool + 1);
    }
    
    return 0;
}

void Thing::register_observer(Names *names, unsigned char *event, EventFunc handler)
{
    Symbol symbol = names->get_symbol(event);
    
    if (symbol)
        register_observer(symbol, handler);
}

void Thing::register_observer(Symbol event, EventFunc handler)
{
    JSON *node = JSON::new_function((GenericFn)handler);
    JSON *events = WebThings::get_json(this->events);
    events->insert_property(event, node);
}

void Thing::set_property(Names *names, unsigned char *property, JSON *value)
{
    Symbol symbol = names->get_symbol(property);
    
    if (symbol)
        set_property(symbol, value);
}

void Thing::set_property(Symbol property, JSON *value)
{
    JSON *properties = WebThings::get_json(this->properties);
    properties->insert_property(property, value);
}

JSON *Thing::get_property(Names *names, unsigned char *property)
{
    Symbol symbol = names->get_symbol(property);
    
    if (symbol)
        return get_property(symbol);
        
    return null;
}

JSON *Thing::get_property(Symbol property)
{
    JSON *properties = WebThings::get_json(this->properties);
    return properties->retrieve_property(property);
}

Thing *Thing::get_thing(Names *names, unsigned char *property)
{
    Symbol symbol = names->get_symbol(property);
    
    if (symbol)
        return get_thing(symbol);
        
    return null;
}
Thing *Thing::get_thing(Symbol property)
{
    JSON *properties = WebThings::get_json(this->properties);
    JSON *json = properties->retrieve_property(property);
    
    if (json)
        return json->get_thing();
        
    return null;
}

void Thing::invoke(Symbol action, ...)
{
    JSON *actions = WebThings::get_json(this->actions);

    GenericFn func  = actions->retrieve_function(action);

    // check to see if there is any data to pass
    // using varargs and possibly the data model
    if (func)
    {
        // cheat for now and assume no data and no response
        (*func)(null);
    }
}

void Proxy::set_property(Names *names, unsigned char *property, JSON *value)
{
    Symbol symbol = names->get_symbol(property);
    
    if (symbol)
        set_property(symbol, value);
}

void Proxy::set_property(Symbol property, JSON *value)
{
    JSON *properties = WebThings::get_json(this->properties);
    properties->insert_property(property, value);
}

JSON *Proxy::get_property(Names *names, unsigned char *property)
{
    Symbol symbol = names->get_symbol(property);
    
    if (symbol)
        return get_property(symbol);
        
    return null;
}

JSON *Proxy::get_property(Symbol property)
{
    JSON *properties = WebThings::get_json(this->properties);
    return properties->retrieve_property(property);
}

Thing *Proxy::get_thing(Names *names, unsigned char *property)
{
    Symbol symbol = names->get_symbol(property);
    
    if (symbol)
        return get_thing(symbol);
        
    return null;
}
Thing *Proxy::get_thing(Symbol property)
{
    JSON *properties = WebThings::get_json(this->properties);
    JSON *json = properties->retrieve_property(property);
    
    if (json)
        return json->get_thing();
        
    return null;
}

void Proxy::invoke(Symbol action, ...)
{
}