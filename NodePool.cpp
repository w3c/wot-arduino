/* a static memory pool for JSON and AvlNode objects */

#include <Arduino.h>
#include "NodePool.h"
#include "AvlNode.h"
#include "Names.h"
#include "JSON.h"
#include "WebThings.h"

WotNodePool::WotNodePool()
{
    used = 0;  // index for allocation next node
    last_allocated = WOT_NODE_POOL_SIZE - 1;
}

unsigned int WotNodePool::size()
{
    return WOT_NODE_POOL_SIZE * sizeof(wot_node_pool_t);
}

float WotNodePool::percent_used()
{
    // return percentage of allocated nodes
    return 100.0 * used / (1.0 * WOT_NODE_POOL_SIZE);
}

// dumb allocator - I should form free nodes into a linked list
void *WotNodePool::allocate_node()
{
    if (used >= WOT_NODE_POOL_SIZE)
        WebThings::collect_garbage();
        
    if (used < WOT_NODE_POOL_SIZE) {
        for (unsigned int i = last_allocated + 1; i < WOT_NODE_POOL_SIZE; ++i) {
            if (wot_pool[i].byte[0] == 0) {
                used++;
                last_allocated = i;
                return (void *)(wot_pool + i);
            }
        }
    }
        
    if (used < WOT_NODE_POOL_SIZE) {
        for (unsigned int i = 0; i < last_allocated + 1; ++i) {
            if (wot_pool[i].byte[0] == 0) {
                used++;
                last_allocated = i;
                return (void *)(wot_pool + i);
            }
        }
    }
    
    // should record this in EEPROM then restart server
    Serial.println(F("Error: exhausted node pool"));
    return 0;
}

void *WotNodePool::get_node(unsigned int index)
{
    if (index < WOT_NODE_POOL_SIZE)
        return (void *)(wot_pool + index);
        
    return 0;
}

wot_node_pool_t *WotNodePool::get_pool()
{
    return wot_pool;
}

void WotNodePool::free(void *node)
{
    // check for valid pointer to node pool and
    // take care to avoid freeing a node twice
    // since we would then mess up the used count
    
    if ((wot_node_pool_t *)node >= wot_pool &&
            (wot_node_pool_t *)node < wot_pool + WOT_NODE_POOL_SIZE &&
            ((char *)node)[0]) {
        memset((char *)node, 0, sizeof(wot_node_pool_t));
        
        if (used)
            --used; 
    }
}

