/* Minimal JSON support for Arduino */

#include "JSON.h"

// initialise memory pool for allocating nodes
JSON *JSON::pool = NULL;
unsigned int JSON::length = 0;
unsigned int JSON::size = 0;

void JSON::initialise_pool(JSON *buffer, unsigned int size)
{
    pool = buffer;  // memory for size nodes
    length = size;  // number of possible nodes
    size = 0;  // index for allocation next node
    memset(buffer, 0, size * sizeof(JSON));
}

JSON * JSON::parse(unsigned char *src, unsigned int length)
{
    JSONLexer lexer;
    lexer.src = src;
    lexer.length = length;
    return parse_private(&lexer);
}

// build JSON object hierarchy
JSON * JSON::parse_private(JSONLexer *lexer)
{
    // check token to determine what JSON type to construct
    return null;
}

// allocate node from fixed memory pool
JSON * JSON::new_node()
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

JSON * JSON::new_float(float n)
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->tag = Float_t;
        node->variant.number = n;
    }
    
    return node;
}

JSON * JSON::new_unsigned(unsigned int n)
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->tag = Unsigned_t;
        node->variant.u = n;
    }
    
    return node;
}

JSON * JSON::new_signed(int n)
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->tag = Signed_t;
        node->variant.i = n;
    }
    
    return node;
}

JSON * JSON::new_boolean(bool value)
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->tag = Boolean_t;
        node->variant.truth = value;
    }
    
    return node;
}

JSON * JSON::new_null()
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->tag = Object_t;
        node->variant.object = null;
    }
    
    return node;
}

JSON * JSON::new_string(unsigned char *str)
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->tag = String_t;
        node->variant.str = str;
    }
    
    return node;
}


JSON * JSON::new_object()
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->tag = Object_t;
        node->variant.object = null;
    }
    
    return node;
}

Json_Tag JSON::json_type()
{
    return this->tag;
}

void JSON::insert(unsigned int symbol, JSON *value)
{
    if (this->tag == Object_t)
        this->variant.object = AvlNode::avlInsertKey(this->variant.object, symbol, value);
}

JSON * JSON::retrieve(unsigned int symbol)
{
    if (this->tag == Object_t)
        return (JSON *)AvlNode::avlFindKey(this->variant.object, (AvlKey)symbol);
        
    return null;
}
