/* a static memory pool for JSON and AvlNode objects */

#include <Arduino.h>
#include "NodePool.h"

WotNodePool::WotNodePool()
{
    wot_pool_size = 0;  // index for allocation next node
}

unsigned int WotNodePool::size()
{
    return WOT_NODE_POOL_SIZE * sizeof(wot_node_pool_t);
}

float WotNodePool::used()
{
    // return percentage of allocated nodes
    return 100.0 * wot_pool_size / (1.0 * WOT_NODE_POOL_SIZE);
}

void *WotNodePool::allocate_node()
{
    if (wot_pool_size < WOT_NODE_POOL_SIZE)
        return (void *)(wot_pool + wot_pool_size++);
        
#ifdef DEBUG
    PRINTLN("out of node space");
#endif
    return 0;
}

wot_node_pool_t *WotNodePool::get_pool()
{
    return wot_pool;
}

