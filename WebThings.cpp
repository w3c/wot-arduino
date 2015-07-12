/* Web of Things Framework for Arduino */

#include <stdarg.h>
#include <Arduino.h>
#include "NodePool.h"
#include "AvlNode.h"
#include "HashTable.h"
#include "JSON.h"
#include "WebThings.h"

#ifndef null
#define null 0
#endif

// static allocation of memory pools
WotNodePool wot_node_pool;

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

void WebThings::thing(const char *name, const char *model, ThingHandler setup)
{

    //PRINT(F("model length = "));
    //PRINTLN((int)strlen(model));

    //return new Thing();
}

void WebThings::register_proxy(const char *uri, ThingHandler setup)
{
    //Thing *proxy = new Thing();
}

void Thing::register_observer(Symbol event, EventHandler handler)
{
}

void Thing::unregister_observer(Symbol event, EventHandler handler)
{
}

void Thing::set_property(Symbol property, Any value)
{
}

Any Thing::get_property(Symbol property)
{
    return null;
}

void Thing::invoke(Symbol action, ...)
{
}

Symbol Thing::get_symbol(const char *name)
{
    return 0;
}

void Proxy::register_observer(Symbol event, EventHandler handler)
{
}

void Proxy::unregister_observer(Symbol event, EventHandler handler)
{
}

void Proxy::set_property(Symbol property, Any value)
{
}

Any Proxy::get_property(Symbol property)
{
    return null;
}

void Proxy::invoke(Symbol action, ...)
{
}

Symbol Proxy::get_symbol(const char *name)
{
    return 0;
}
