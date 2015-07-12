/* a registry of things hosted on this server and proxies for remote things */

#include <Arduino.h>
#include "NodePool.h"
#include "WebThings.h"
#include "Registry.h"

// initialise memory pool for allocating nodes

static WotNodePool *node_pool_manager;
static wot_node_pool_t *node_pool;
static RIndex things, proxies;

#define RNODE(i) ((RNode *)(node_pool + i - 1))
#define RINDEX(node) ((wot_node_pool_t *)node - node_pool + 1);

void Registry::initialise_pool(WotNodePool *pool)
{
    node_pool_manager = pool;
    node_pool = pool->get_pool();
}

Thing *Registry::find_thing(const char *uri, uint16_t length, String_Tag tag)
{
    RNode *node = RNODE(things);
    
    while (node)
    {
        if (!node->match(uri, length, tag))
            return (Thing *)RNODE(node->index);
            
        node = RNODE(node->next);
    }
    
    return null;
}

Proxy *Registry::find_proxy(const char *uri, uint16_t length, String_Tag tag)
{
    RNode *node = RNODE(proxies);
    
    while (node)
    {
        if (!node->match(uri, length, tag))
            return (Proxy *)RNODE(node->index);
            
        node = RNODE(node->next);
    }
    
    return null;
}

void Registry::register_thing(const char *uri, uint16_t length, String_Tag tag, Thing *thing)
{
    Thing *t = find_thing(uri, length, tag);
    
    if (!t)
    {
        RNode * node = (RNode *)(node_pool_manager->allocate_node());
    
        if (node)
        {
            node->index = RINDEX(thing);
            node->set_tag(tag);
            node->set_str_length(length);
            node->uri = uri;
            node->next = things;
            things = RINDEX(node);
        }
        else
        {
#ifdef DEBUG
            PRINTLN("insufficient space to register thing");
#endif
        }
    }
}

void Registry::register_proxy(const char *uri, uint16_t length, String_Tag tag, Proxy *proxy)
{
    Proxy *t = find_proxy(uri, length, tag);
    
    if (!t)
    {
        RNode * node = (RNode *)(node_pool_manager->allocate_node());
    
        if (node)
        {
            node->index = RINDEX(proxy);
            node->set_tag(tag);
            node->set_str_length(length);
            node->uri = uri;
            node->next = proxies;
            proxies = RINDEX(node);
        }
        else
        {
#ifdef DEBUG
            PRINTLN("insufficient space to register proxy");
#endif
        }
    }
}

// essentially strncmp() - compares two strings with given lengths
// can be customised to work with strings in SRAM, EEPROM or Flash
// based upon String tags which tell you where a string is stored

int Registry::RNode::match(const char *str, uint16_t length, String_Tag tag)
{
    const char *s1 = (char *)str;
    unsigned int len1 = length;
    const char *s2 = (char *)uri;
    unsigned int len2 = get_str_length();
    
    if (len1 && len2)
    {
        while(len1-- && len2-- && (*s1==*s2))
        {
            s1++;
            s2++;
        }
    
        return *s1-*s2;
    }
        
    if (!(len1 | len2))
        return 0;

    return (len1 ? 1 : -1);
}

String_Tag Registry::RNode::get_tag()
{
    return (String_Tag)(taglen & 3);
}

void Registry::RNode::set_tag(String_Tag tag)
{
    taglen &= ~3;
    taglen |= (unsigned int)tag;
}
void Registry::RNode::set_str_length(unsigned int length)
{
    taglen &= 3;
    taglen |= (length << 2);
}

unsigned int Registry::RNode::get_str_length()
{
    return taglen >> 2;
}
