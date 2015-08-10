/* Minimal JSON support for Arduino */

#include <ctype.h>
#include <Arduino.h>
#include "NodePool.h"
#include "AvlNode.h"
#include "Names.h"
#include "JSON.h"

static WotNodePool *node_pool;

void JSON::initialise_pool(WotNodePool *wot_node_pool)
{
    node_pool = wot_node_pool;
}

JSON * JSON::parse(const char *src, Names *names)
{
    return parse(src, strlen(src), names);
}

JSON * JSON::parse(const char *src, unsigned int length, Names *names)
{
    Lexer lexer;
    lexer.src = (unsigned char *)src;
    lexer.length = length;
    lexer.names = names;
    
#ifdef DEBUG
    PRINT(F("parsing ")); PRINTLN(src);
#endif
    return parse_private(&lexer);
}

// build JSON object hierarchy
JSON * JSON::parse_private(Lexer *lexer)
{
    // check token to determine what JSON type to construct
    Json_Token token = lexer->get_token();
    
    switch (token)
    {
        case Object_start_token:
            return parse_object(lexer);
        case Array_start_token:
            return parse_array(lexer);
        case String_token:
            return new_string(lexer->token_src, lexer->token_len);
        case Null_token:
            return new_null();
        case True_token:
            return new_boolean(true);
        case False_token:
            return new_boolean(false);
        case Float_token:
            return new_float(lexer->float_num);
        case Unsigned_token:
            return new_unsigned(lexer->unsigned_num);
        case Signed_token:
            return new_signed(lexer->signed_num);
        default:
#ifdef DEBUG
            PRINTLN(F("JSON syntax error"));
#endif
            return null;
    }
    
    token = lexer->get_token();
    
    if (token != Error_token)
    {
#ifdef DEBUG
        PRINTLN(F("JSON syntax error"));
#endif
        return null;
    }

    return null;
}

JSON * JSON::parse_object(Lexer *lexer)
{
    unsigned int symcount = JSON_SYMBOL_BASE;
    JSON *object = new_object();
    Json_Token token = lexer->get_token();
    
    while (token != Error_token)
    {
        if (token == Object_stop_token)
            return object;
            
        if (token != String_token)
            break;
            
        unsigned int symbol = lexer->names->get_symbol(lexer->token_src,
                                        lexer->token_len, &symcount);
                                        
        token = lexer->get_token();
        
        if (token != Colon_token)
            break;
        
        JSON *value = parse_private(lexer);
        
        if (!value)
            break;
            
        object->insert_property(symbol, value);
        token = lexer->get_token();
        
        if (token == Object_stop_token)
            continue;
            
        if (token != Comma_token)
            break;
            
        token = lexer->get_token();
    }
    
    // free incomplete object here along with its map from symbols to values
    
#ifdef DEBUG
    PRINT(F("JSON syntax error in object, token is ")); PRINTLN(token);
#endif
    return null;
}

JSON * JSON::parse_array(Lexer *lexer)
{
    JSON *array = new_array();
    unsigned int index = 0;
    
    if (lexer->end_of_array())
        return array;  // empty array
        
    for (;;)
    {
        JSON *item = parse_private(lexer);
    
        if (item)
            array->insert_array_item(index++, item);
        else
        {
#ifdef DEBUG
            PRINTLN(F("missing array item"));
#endif
            break;
        }
 
        Json_Token token = lexer->get_token();
        
        if (token == Array_stop_token)
            return array;
            
        if (token != Comma_token)
            break;
    }

#ifdef DEBUG
    PRINTLN(F("JSON syntax error in array"));
#endif
    return null;
}

#ifdef DEBUG

void JSON::print_string(const unsigned char *name, unsigned int length)
{
    int i;
    
    PRINT("\"");

    for (i = 0; i < length; ++i)
        PRINT(((const char)name[i]));
        
    PRINT(F("\""));
}

void JSON::print_name_value(AvlKey key, AvlValue value, void *context)
{
    PRINT(F(" ")); PRINT((unsigned int)key); PRINT(F(" : "));
    ((JSON *)value)->print();
    
    if ((void *)value != context)
        PRINT(F(","));
}

void JSON::print_array_item(AvlKey key, AvlValue value, void *context)
{
    ((JSON *)value)->print();
    
    if ((void *)value != context)
        PRINT(F(","));
}

void JSON::print()
{
    switch (get_tag())
    {
        case Object_t:
            PRINT(F(" {"));
            AvlNode::apply(this->variant.object, (AvlApplyFn)print_name_value,
                 (JSON *)(AvlNode::get_node(AvlNode::last(this->variant.object)))->get_value());
            PRINT(F("} "));
            break;
            
        case Array_t:
            PRINT(F(" [ "));
            AvlNode::apply(this->variant.object, (AvlApplyFn)print_array_item,
                 (JSON *)(AvlNode::get_node(AvlNode::last(this->variant.object)))->get_value());
            PRINT(F("] "));
            break;
            
        case String_t:
            print_string(variant.str, get_str_length());
            break;
            
        case Unsigned_t:
            PRINT(this->variant.u);
            break;
        case Signed_t:
            PRINT(this->variant.i);
            break;
        case Float_t:
            PRINT(this->variant.number);
            break;
                    
        case Boolean_t:
            PRINT((this->variant.truth ? F(" true ") : F(" false ")));
            break;
            
        case Null_t:
            PRINT(F(" null "));
            break;
            
        case Function_t:
            PRINT(F(" function "));
            break;
            
        case Proxy_t:
            PRINT(F(" proxy "));
            break;
            
        case Thing_t:
            PRINT(F(" thing "));
            break;
            
        case Unused_t:
            break;  // nothing to do
    }
}

#endif

// allocate node from fixed memory pool
JSON * JSON::new_node()
{
    JSON * node = (JSON *)(node_pool->allocate_node());
    
    if (node)
        node->taglen = 0;
    
    return node;
}

JSON * JSON::new_float(float n)
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->set_tag(Float_t);
        node->variant.number = n;
    }
    
    return node;
}

JSON * JSON::new_unsigned(unsigned int n)
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->set_tag(Unsigned_t);
        node->variant.u = n;
    }
    
    return node;
}

JSON * JSON::new_signed(int n)
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->set_tag(Signed_t);
        node->variant.i = n;
    }
    
    return node;
}

JSON * JSON::new_boolean(bool value)
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->set_tag(Boolean_t);
        node->variant.truth = value;
    }
    
    return node;
}

JSON * JSON::new_null()
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->set_tag(Null_t);
    }
    
    return node;
}

JSON * JSON::new_string(unsigned char *str, unsigned int length)
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->set_tag(String_t);
        node->variant.str = str;
        node->set_str_length(length);
    }
    
    return node;
}


JSON * JSON::new_object()
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->set_tag(Object_t);
        node->variant.object = 0;
    }
    
    return node;
}

// arrays are not yet implemented
// need to decide how to store them!
JSON * JSON::new_array()
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->set_tag(Array_t);
        node->variant.object = 0;
    }
    
    return node;
}

JSON * JSON::new_function(GenericFn func)
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->set_tag(Function_t);
        node->variant.function = func;
    }
    
    return node;
}

Json_Tag JSON::json_type()
{
    return get_tag();
}

Json_Tag JSON::get_tag()
{
    return (Json_Tag)(taglen & 15);
}

void JSON::set_tag(Json_Tag tag)
{
    taglen &= ~15;
    taglen |= (unsigned int)tag;
}
void JSON::set_str_length(unsigned int length)
{
    taglen &= 15;
    taglen |= (length << 4);
}

unsigned int JSON::get_str_length()
{
    return taglen >> 4;
}

boolean JSON::is_null()
{
    return (get_tag() == Null_t);
}

boolean JSON::get_boolean()
{
    if (get_tag() == Boolean_t)
        return variant.truth;
        
    return false;
}

unsigned char * JSON::get_string(unsigned int *len)
{
    if (len)
        *len = get_str_length();
        
    return variant.str;
}

unsigned int JSON::get_unsigned()
{
    if (get_tag() == Unsigned_t)
        return variant.u;
        
    return 0;
}

int JSON::get_signed()
{
    if (get_tag() == Signed_t)
        return variant.i;
        
    return 0;
}

float JSON::get_float()
{
    if (get_tag() == Float_t)
        return variant.number;
        
    return 0.0;
}

Thing *JSON::get_thing()
{
    if (get_tag() == Thing_t)
        return variant.thing;
        
    return null;
}

Proxy *JSON::get_proxy()
{
    if (get_tag() == Proxy_t)
        return variant.proxy;
        
    return null;
}

JSON * JSON::retrieve_property(unsigned int symbol)
{
    AvlKey key = (AvlKey)symbol + 1;
    
    if (get_tag() == Object_t)
        return (JSON *)AvlNode::find_key(variant.object, key);
        
    return null;
}

JSON * JSON::retrieve_array_item(unsigned int index)
{
    AvlKey key = (AvlKey)index + 1;

    if (get_tag() == Array_t)
        return (JSON *)AvlNode::find_key(variant.object, key);
        
    return null;
}

GenericFn JSON::retrieve_function(Symbol action)
{
    if (get_tag() == Object_t)
    {
        JSON *func = (JSON *)AvlNode::find_key(variant.object, action);
        
        if (func && func->get_tag() == Function_t)
            return func->variant.function;
    }
    
    return null;
}

// *** fix me to deal with updating existing values
// *** so that we don't leak memory through lost nodes
void JSON::insert_property(unsigned int symbol, JSON *value)
{
    AvlKey key = (AvlKey)symbol + 1;
    
    if (get_tag() == Object_t)
        variant.object = AvlNode::insert_key(variant.object, key, (void *)value);
}

// Prepending is a lot harder and could be done
// by appending a copy of the last node value then
// shifting the values from one node to the next
// in a left to right traversal, and finally
// updating the first node. This will require a
// special helper in the AvlNode class.

void JSON::append_array_item(JSON *value)
{
    if (get_tag() == Array_t)
    {
        AvlKey last = AvlNode::last_key(variant.object);
        insert_array_item((unsigned int)last, value);
    }
}

// *** fix me to deal with updating existing values
// *** so that we don't leak memory through lost nodes
void JSON::insert_array_item(unsigned int index, JSON *value)
{
    AvlKey key = (AvlKey)index + 1;
    
    if (get_tag() == Array_t)
        variant.object =  AvlNode::insert_key(variant.object, key, (void *)value);
}

boolean JSON::Lexer::end_of_array()
{
    unsigned int c = peek_byte();
    
    while (isspace(c))
    {
        next_byte();
        c = peek_byte();
    }

    if (c == ']')
        return true;
        
    return false;
}

Json_Token JSON::Lexer::get_token()
{
    unsigned int c = peek_byte();
    
    while (isspace(c))
    {
        next_byte();
        c = peek_byte();
    }
    
    switch (c)
    {
        case ':':
            next_byte();
            return Colon_token;
        case ',':
            next_byte();
            return Comma_token;
        case '{':
            next_byte();
            return Object_start_token;
        case '}':
            next_byte();
            return Object_stop_token;
        case '[':
            next_byte();
            return Array_start_token;
        case ']':
            next_byte();
            return Array_stop_token;
        case '"':
            return get_string();
        case '-':
        case '.':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return get_number(c);
        default: 
            return get_special(c);
    }
}

// signed and unsigned integers, and floats
Json_Token JSON::Lexer::get_number(unsigned int c)
{
    boolean negative = false;
    boolean real = false;
    int d = 0, e = 0;
    token_src = src;
    token_len = 0;
    
    if (c == '-')
    {
        negative = true;
        next_byte();
        c = peek_byte();
    }
    
    if (c == '.')
    {
        next_byte();
        real = true;
        c = peek_byte();
    }
    
    while (isdigit(c))
    {
        ++d;
        next_byte();
        c = peek_byte();
    }
    
    if (c == '.')
    {
        next_byte();
        real = true;
        c = peek_byte();
    }
    
    while (isdigit(c))
    {
        ++d;
        next_byte();
        c = peek_byte();
    }
    
    if (c == 'e' || c == 'E')
    {
        if (!d)
            return Error_token;
            
        next_byte();
        real = true;
        c = peek_byte();
    }
    
    if (c == '-')
    {
        next_byte();
        c = peek_byte();
    }

    while (isdigit(c))
    {
        ++e;
        next_byte();
        c = peek_byte();
    }
    
    if (! (d|e))
        return Error_token;
        
    if (real)
    {
        sscanf((const char *)token_src, "%f", &float_num);
        return Float_token;
    }
 
    if (negative)
    {
        sscanf((const char *)token_src, "%d", &signed_num);
        return Signed_token;
    }
    
    sscanf((const char *)token_src, "%u", &unsigned_num);
    return Unsigned_token;
}

// null, true or false
Json_Token JSON::Lexer::get_special(unsigned int c)
{
    if (c == 'n')
    {
        if (length >= 4 &&
            src[1] == 'u' &&
            src[2] == 'l' &&
            src[3] == 'l')
        {
            src += 4;
            length += 4;
            return Null_token;
        }
    }
    else if (c == 't')
    {
        if (length >= 4 &&
            src[1] == 'r' &&
            src[2] == 'u' &&
            src[3] == 'e')
        {
            src += 4;
            length += 4;
            return True_token;
        }
    }
    else if (c == 'f')
    {
        if (length >= 5 &&
            src[1] == 'a' &&
            src[2] == 'l' &&
            src[3] == 's' &&
            src[4] == 'e')
        {
            src += 5;
            length += 5;
            return False_token;
        }
    }
    
    return Error_token;
}

Json_Token JSON::Lexer::get_string()
{
    next_byte();
    token_src = src;
    token_len = 0;
    
    while (length > 0)
    {
        --length;
        
        if (*src++ == '"')
            return String_token;

        ++token_len;
    }
    
    return Error_token;
}

void JSON::Lexer::next_byte()
{
    if (length > 0)
    {
        --length;
        src++;
    }
}

unsigned int JSON::Lexer::peek_byte()
{
    if (length > 0)
        return *src;
    
    return 256;
}
