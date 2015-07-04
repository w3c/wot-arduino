/* Web of Things Framework for Arduino */

#include <stdarg.h>
#include <Arduino.h>
#include "AvlNode.h"
#include "HashTable.h"
#include "JSON.h"
#include "WebThings.h"

// static allocation of memory pools

AvlNode avlNodePool[AVL_NODE_POOL_SIZE];
JSON jsonNodePool[JSON_NODE_POOL_SIZE];

WebThings::WebThings()
{
    AvlNode::initialise_pool(&(avlNodePool[0]), AVL_NODE_POOL_SIZE);
    JSON::initialise_json_pool(&(jsonNodePool[0]), JSON_NODE_POOL_SIZE);
    
    PRINT(F("avl node size: "));
    PRINT((sizeof(AvlNode)));
    PRINTLN(F(" bytes"));
    
    PRINT(F("json node size: "));
    PRINT((sizeof(JSON)));
    PRINTLN(F(" bytes"));
    
    PRINT(F("avl pool size: "));
    PRINT((AVL_NODE_POOL_SIZE * sizeof(AvlNode)));
    PRINTLN(F(" bytes"));
    
    PRINT(F("json pool size: "));
    PRINT((JSON_NODE_POOL_SIZE * sizeof(JSON)));
    PRINTLN(F(" bytes"));
}

Thing * WebThings::thing(const char *name, const char *model)
{
    return new Thing();
}

void WebThings::register_proxy(const char *uri, ThingHandler succeed, ErrorHandler fail)
{
    Thing *proxy = new Thing();
    succeed(proxy);
}

void Thing::register_observer(const char *name, EventHandler handler)
{
}

void Thing::unregister_observer(const char *name, EventHandler handler)
{
}

void Thing::set_property(const char *name, Any value)
{
}

Any Thing::get_property(const char *name)
{
    return null;
}

void Thing::invoke(const char *name, ...)
{
}

void Proxy::register_observer(const char *name, EventHandler handler)
{
}

void Proxy::unregister_observer(const char *name, EventHandler handler)
{
}

void Proxy::set_property(const char *name, Any value)
{
}

Any Proxy::get_property(const char *name)
{
    return null;
}

void Proxy::invoke(const char *name, ...)
{
}
