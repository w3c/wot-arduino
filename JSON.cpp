/* Minimal JSON support for Arduino */

#include <iostream>

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
    memset(buffer, size * sizeof(JSON), 0);
}

// allocate node from fixed memory pool
JSON * JSON::newNode()
{
    JSON * node = NULL;
    
    if (pool && size < length)
    {
        node = pool + size++;
        node->tag = Null;
        node->variant.number = 0.0;
    }
    
    return node;
}

JSON * JSON::newNumber(float x)
{
    JSON *node = JSON::newNode();
    
    if (node)
    {
        node->tag = Number;
        node->variant.number = x;
    }
    
    return node;
}

JSON * JSON::newBoolean(bool value)
{
    JSON *node = JSON::newNode();
    
    if (node)
    {
        node->tag = Boolean;
        node->variant.truth = value;
    }
    
    return node;
}

JSON * JSON::newNull()
{
    JSON *node = JSON::newNode();
    
    if (node)
    {
        node->tag = Object;
        node->variant.object = null;
    }
    
    return node;
}

JSON * JSON::newString(string& str)
{
    JSON *node = JSON::newNode();
    
    if (node)
    {
        node->tag = String;
        node->variant.str = &str;
    }
    
    return node;
}


JSON * JSON::newObject()
{
    JSON *node = JSON::newNode();
    
    if (node)
    {
        node->tag = Object;
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

JSON * JSON::parse(string src)
{
    // *** Implement me ***
    return (JSON *)0;
}

void JSON::to_string(string &str)
{
    // not sure what to do given issues with dynamic strings
    // guess that we need a static char *buffer to write to 
}

void JSON::insert(string &name, JSON *value)
{
    if (this->tag == String)
        this->variant.object = AvlNode::avlInsertKey(this->variant.object, name, value);
}

JSON * JSON::retrieve(string& name)
{
    if (this->tag == Object)
        return (JSON *)AvlNode::avlFindKey(this->variant.object, (AvlKey)name);
        
    return null;
}
