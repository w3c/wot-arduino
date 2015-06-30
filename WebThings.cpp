/* Web of Things Framework for Arduino */

#include <iostream>
using namespace std;

#include "core.h"
#include "WebThings.h"

// static allocation of memory pools

AvlNode avlNodePool[AVL_NODE_POOL_SIZE];
JSON jsonNodePool[JSON_NODE_POOL_SIZE];

WebThings::WebThings()
{
    AvlNode::initialise_pool(&(avlNodePool[0]), AVL_NODE_POOL_SIZE);
    JSON::initialise_pool(&(jsonNodePool[0]), JSON_NODE_POOL_SIZE);
}

Thing * WebThings::thing(string name, string model)
{
    return new Thing();
}

void WebThings::register_proxy(string uri, ThingHandler succeed, ErrorHandler fail)
{
    Thing *proxy = new Thing();
    succeed(proxy);
}

void Thing::register_observer(string name, EventHandler handler)
{
}

void Thing::unregister_observer(string name, EventHandler handler)
{
}

void Thing::set_property(string name, Any value)
{
}

Any Thing::get_property(string name)
{
    return null;
}

void Thing::invoke(string name, ...)
{
}

void Proxy::register_observer(string name, EventHandler handler)
{
}

void Proxy::unregister_observer(string name, EventHandler handler)
{
}

void Proxy::set_property(string name, Any value)
{
}

Any Proxy::get_property(string name)
{
    return null;
}

void Proxy::invoke(string name, ...)
{
}
