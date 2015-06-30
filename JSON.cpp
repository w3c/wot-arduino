/* Minimal JSON support for Arduino */

#include <iostream>
#include <string.h>

#include "JSON.h"
#include "HashTable.h"

// initialise memory pool for allocating nodes
JSON *JSON::pool = NULL;
unsigned int JSON::length = 0;
unsigned int JSON::size = 0;

void JSON::initialise_pool(JSON *buffer, unsigned int size)
{
    pool = buffer;  // memory for size nodes
    length = size;  // number of possible nodes
    size = 0;  // index for allocation next node
    std::memset(buffer, size * sizeof(JSON), 0);
}

// allocate node from fixed memory pool
JSON * JSON::newNode()
{
    JSON * node = NULL;
    
    if (pool && size < length)
    {
        node = pool + size++;
        node->tag = Null_t;
        node->variant.number = 0.0;
    }
    
    return node;
}

JSON * JSON::newNumber(float x)
{
    JSON *node = JSON::newNode();
    
    if (node)
    {
        node->tag = Number_t;
        node->variant.number = x;
    }
    
    return node;
}

JSON * JSON::newBoolean(bool value)
{
    JSON *node = JSON::newNode();
    
    if (node)
    {
        node->tag = Boolean_t;
        node->variant.truth = value;
    }
    
    return node;
}

JSON * JSON::newNull()
{
    JSON *node = JSON::newNode();
    
    if (node)
    {
        node->tag = Object_t;
        node->variant.object = null;
    }
    
    return node;
}

JSON * JSON::newString(String str)
{
    JSON *node = JSON::newNode();
    
    if (node)
    {
        node->tag = String_t;
        node->variant.str = str;
    }
    
    return node;
}


JSON * JSON::newObject()
{
    JSON *node = JSON::newNode();
    
    if (node)
    {
        node->tag = Object_t;
        node->variant.object = null;
    }
    
    return node;
}

JSON * JSON::newArray(unsigned int size)
{
    // Help! I don't know how to safely allocate arrays
    // dynamically on constrained system like the Arduino Uno
    return null;
}

string JSON::stringify(JSON& obj)
{
    // Not sure what to do given issues with dynamic strings
    // guess that we need a static char *buffer to write to 
    return "hello world";
}

JSON * JSON::parse(String src)
{
    // *** Implement me ***
    return (JSON *)0;
}

void JSON::to_string(String str)
{
    // not sure what to do given issues with dynamic strings
    // guess that we need a static char *buffer to write to 
}

void JSON::insert(String name, JSON *value)
{
    if (this->tag == String_t)
        this->variant.object = AvlNode::avlInsertKey(this->variant.object, name, value);
}

JSON * JSON::retrieve(String name)
{
    if (this->tag == Object_t)
        return (JSON *)AvlNode::avlFindKey(this->variant.object, (AvlKey)name);
        
    return null;
}
