/* WebCore.cpp - allocating thing & proxy objects from a static pool */

#include <Arduino.h>
#include "NodePool.h"
#include "AvlNode.h"
#include "Names.h"
#include "JSON.h"
#include "WebThings.h"

static Thing thing_pool[MAX_THINGS];
static unsigned int things_count;

CoreThing *ThingPool::allocate()
{
    for (unsigned int i = 0; i < MAX_THINGS; ++i) {
        if (thing_pool[i].id == 0)
            return (CoreThing *)(thing_pool + i);
    }
    
    return (CoreThing *)0;
}

void ThingPool::free(CoreThing *thing)
{
    if (thing->id != 0) {
        ++things_count;
        thing_pool->id = 0;
    }
}

unsigned int ThingPool::size()
{
    return things_count;
}

float ThingPool::used()
{
    // return percentage of allocated nodes
    return 100.0 * things_count / (1.0 * MAX_THINGS);
}
