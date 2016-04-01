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

// static allocation of memory to avoid new/free fragmentation

static WotNodePool wot_node_pool; // for allocation of JSON and AVL tree nodes
static ThingPool thing_pool; // for allocation of thing and proxy objects
static NPIndex stale[MAX_STALE]; // for tracking overwritten JSON references
static unsigned int stale_count; // number of stale references
static boolean gc_phase; // alternates between odd and even on successive cycles
static Thing *things;  // linked list of things hosted on this server
static Proxy *proxies;  // linked list of proxies for things on other servers


WebThings::WebThings()
{
    Serial.print(F("stale count ")); Serial.println(stale_count);
    AvlNode::initialise_pool(&wot_node_pool);
    JSON::initialise_pool(&wot_node_pool);
    
    Serial.print(F("avl node size: "));
    Serial.print((sizeof(AvlNode)));
    Serial.println(F(" bytes"));
    
    Serial.print(F("json node size: "));
    Serial.print((sizeof(JSON)));
    Serial.println(F(" bytes"));
    
    Serial.print(F("pool node size: "));
    Serial.print((sizeof(wot_node_pool_t)));
    Serial.println(F(" bytes"));
    
    Serial.print(F("WoT pool size: "));
    Serial.print(wot_node_pool.size());
    Serial.println(F(" bytes"));
    
    gc_phase = 0;
    JSON::set_gc_phase(gc_phase);
}

unsigned int WebThings::used()
{
    return wot_node_pool.used;
}

void WebThings::add_stale(JSON *json)
{
    // free unless its an object or array
    if (!json->free_leaves()) {
        NPIndex index = get_index(json);
    
        // check if already in the stale set
        for (unsigned int i = 0; i < stale_count; ++i) {
            if (stale[i] == index)
                return;
        }
        
        if (stale_count >= MAX_STALE)
            collect_garbage();
            
        if (stale_count < MAX_STALE)
            stale[stale_count++] = index;
        else
            Serial.println(F("Exhausted room in stale set"));
            
    }
}

void WebThings::collect_garbage()
{
    if (stale_count) {
        // mark all nodes reachable from the roots
        for (Thing *t = things; t; t = (Thing *)t->next)
            t->reachable(gc_phase);
/*
        for (Proxy *proxy = proxies; proxy; proxy = (Proxy *)proxy->next) {
            reachable(proxy->properties);
            reachable(proxy->proxies);
        }
*/

        // now sweep through the nodes reachable from the stale list
        // and delete the ones that aren't reachable from the roots
        for (unsigned int i = 0; i < stale_count; ++i) {
            JSON *json = get_json(stale[i]);
        
            if (!json->marked(gc_phase)) {
                json->sweep(gc_phase);
                for (unsigned int j = i; j < stale_count - 1; ++j)
                    stale[j] = stale[j+1];
                --stale_count;
            }
        }
        
        gc_phase = !gc_phase;
        JSON::set_gc_phase(gc_phase); // to ensure new nodes are marked correctly
    }
}

void WebThings::reachable(NPIndex index)
{
    JSON *json = get_json(index);
    
    if (json)
        json->reachable(gc_phase);
}

void WebThings::remove_thing(Thing *thing)
{
    Thing *t, *next;

    if (things == thing) {
        things = (Thing *)thing->next;
    } else {
        for (t = things; t; t = next) {
            next = (Thing *)t->next;
            
            if (next == thing) {
                t->next = thing->next;
                break;
            }
        }
    }
    
    thing->remove();
}

void WebThings::remove_proxy(Proxy *proxy)
{
    Proxy *p, *next;

    if (proxies == proxy) {
        proxies = (Proxy *)proxy->next;
    } else {
        for (p = proxies; p; p = next) {
            next = (Proxy *)p->next;
            
            if (next == proxy) {
                p->next = proxy->next;
                break;
            }
        }
    }
    
    proxy->remove();
}

#if defined(pgm_read_byte)
void WebThings::thing(const char *name, const __FlashStringHelper *model, SetupFunc setup)
{
    thing(name, ((char *)model)+PROGMEM_BOUNDARY, setup);
}
#endif

void WebThings::thing(const char *name, char *model, SetupFunc setup)
{
    Names table;
    unsigned int id = 0;
    Thing *t = things, *thing = (Thing *)ThingPool::allocate();
    
    if (thing)
    {
        while (t)
        {
            ++id;
            t = (Thing *)t->next;
        }
    
        thing->uri = (char *)name;
        thing->id = ++id;
        thing->model = get_index(JSON::parse(model, &table));
        thing->events = get_index(JSON::new_object());
        thing->properties = get_index(JSON::new_object());
        thing->actions = get_index(JSON::new_object());
        thing->proxies = get_index(JSON::new_array());
    
        thing->next = (Thing *)things;
        things = thing;
        setup(thing, &table);
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

void Thing::print()
{
    Serial.print(F(" model: "));
    WebThings::get_json(this->model)->print();
    Serial.print(F("\n properties: "));
    WebThings::get_json(this->properties)->print();
    Serial.print(F("\n actions: "));
    WebThings::get_json(this->actions)->print();
    Serial.print(F("\n events: "));
    WebThings::get_json(this->events)->print();
    Serial.print(F("\n"));
}

// this ignores the need to inform the proxy chain
void Thing::set_property(Symbol property, JSON *value)
{
    JSON *properties = WebThings::get_json(this->properties);
    properties->insert_property(property, value);
}

JSON *Thing::get_property(Symbol property)
{
    JSON *properties = WebThings::get_json(this->properties);
    return properties->retrieve_property(property);
}

// set the handler for implementing the named action
void Thing::register_action_handler(Symbol action, ActionFunc handler)
{
    JSON *func = JSON::new_function((GenericFn)handler);
    JSON *actions = WebThings::get_json(this->actions);
    actions->insert_property(action, func);
}

int Thing::invoke(Symbol action, ...)
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
    
    return -1;
}

void Thing::register_observer(Symbol event, EventFunc handler)
{
    JSON *func = JSON::new_function((GenericFn)handler);
    JSON *events = WebThings::get_json(this->events);
    events->insert_property(event, func);
}

void Thing::raise_event(Symbol event, ...)
{
}

void Thing::reachable(boolean phase)
{
    WebThings::get_json(properties)->reachable(phase);
    WebThings::get_json(proxies)->reachable(phase);
}

// called when sweep comes across a JSON node that is a Thing
void Thing::sweep(boolean phase)
{
    WebThings::get_json(properties)->sweep(phase);
}

void Thing::remove()
{
    AvlNode::free(properties);
    AvlNode::free(actions);
    AvlNode::free(events);
    ThingPool::free(this);
}

// this needs to forward the new value along the
// proxy chain to the thing which in turn will notify
// all proxies when the value has actually been applied
void Proxy::set_property(Symbol property, JSON *value)
{
    JSON *properties = WebThings::get_json(this->properties);
    JSON *old_value = properties->retrieve_property(property);
    
    // reference to old value will become stale
    if (value != old_value)
        WebThings::add_stale(old_value);

    properties->insert_property(property, value);
}

// this gets the locally cached value which is not
// guaranteed to be be up to date as that isn't
// possible in a distributed system with inevitable
// delays for messages that depend on the protocols,
// communication patterns, and the time for a device
// to wake up from a power preserving mode
JSON *Proxy::get_property(Symbol property)
{
    JSON *properties = WebThings::get_json(this->properties);
    return properties->retrieve_property(property);
}

// set the response handler for the named action
void Proxy::register_response_handler(Symbol action, ResponseFunc handler)
{
    JSON *func = JSON::new_function((GenericFn)handler);
    JSON *actions = WebThings::get_json(this->actions);
    actions->insert_property(action, func);
}

// forward action & data along proxy chain to thing
// need to provide id so that responses can be matched
// note that there may be zero, one or more responses
int Proxy::invoke(Symbol action, ...)
{
    return -1;
}

void Proxy::register_observer(Symbol event, EventFunc handler)
{
    JSON *node = JSON::new_function((GenericFn)handler);
    JSON *events = WebThings::get_json(this->events);
    events->insert_property(event, node);
}

void Proxy::raise_event(Symbol event, ...)
{
}

void Proxy::reachable(boolean phase)
{
    WebThings::get_json(properties)->reachable(phase);
    WebThings::get_json(proxies)->reachable(phase);
}

void Proxy::sweep(boolean phase)
{
    WebThings::get_json(properties)->sweep(phase);
}

void Proxy::remove()
{
    AvlNode::free(properties);
    AvlNode::free(actions);
    AvlNode::free(events);
    ThingPool::free(this);
}
