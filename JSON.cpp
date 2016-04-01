/* Minimal JSON support for Arduino */

#include <ctype.h>
#include <Arduino.h>
#include "Strings.h"
#include "NodePool.h"
#include "AvlNode.h"
#include "Names.h"
#include "JSON.h"
#include "WebThings.h"

static WotNodePool *node_pool;
static boolean gc_phase;

void JSON::initialise_pool(WotNodePool *wot_node_pool)
{
    node_pool = wot_node_pool;
}

#if defined(pgm_read_byte)
JSON * JSON::parse(const __FlashStringHelper *src, Names *table)
{
    Lexer lexer;
    lexer.src = ((char *)src)+PROGMEM_BOUNDARY;
    lexer.length = Strings::strlen(lexer.src);
    lexer.table = table;
        
    return parse_private(&lexer);
}
#endif

JSON * JSON::parse(const char *src, Names *table)
{
    Lexer lexer;
    lexer.src = src;
    lexer.length = Strings::strlen(src);
    lexer.table = table;
    
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
            Serial.println(F("JSON syntax error"));
    }

    return null;
}

JSON * JSON::parse_object(Lexer *lexer)
{
    JSON *object = new_object();
    Json_Token token = lexer->get_token();
    
    while (token != Error_token)
    {
        if (token == Object_stop_token)
            return object;
            
        if (token != String_token)
            break;
            
        Symbol symbol = lexer->table->symbol(lexer->token_src,
                                        lexer->token_len);
                                        
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

    Serial.print(F("JSON syntax error in object, token is ")); Serial.println(token);
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
            Serial.println(F("missing array item"));
            break;
        }
 
        Json_Token token = lexer->get_token();
        
        if (token == Array_stop_token)
            return array;
            
        if (token != Comma_token)
            break;
    }

    Serial.println(F("JSON syntax error in array"));
    return null;
}

void JSON::print_string(const char *name, unsigned int length)
{
    unsigned int i;
    
    Serial.print(F("\""));

    for (i = 0; i < length; ++i)
        Serial.print((Strings::get_char(name++)));
        
    Serial.print(F("\""));
}

void JSON::print_name_value(AvlKey key, AvlValue value, void *context)
{
    Serial.print(F(" ")); Serial.print((unsigned int)key); Serial.print(F(" : "));
    ((JSON *)value)->print();
    
    if ((void *)value != context)
        Serial.print(F(","));
}

void JSON::print_array_item(AvlKey key, AvlValue value, void *context)
{
    ((JSON *)value)->print();
    
    if ((void *)value != context)
        Serial.print(F(","));
}

void JSON::print()
{
    switch (get_tag())
    {
        case Object_t:
            Serial.print(F(" {"));
            AvlNode::apply(this->variant.object, (AvlApplyFn)print_name_value,
                 (JSON *)(AvlNode::get_node(AvlNode::last(this->variant.object)))->get_value());
            Serial.print(F("} "));
            break;
            
        case Array_t:
            Serial.print(F(" [ "));
            AvlNode::apply(this->variant.object, (AvlApplyFn)print_array_item,
                 (JSON *)(AvlNode::get_node(AvlNode::last(this->variant.object)))->get_value());
            Serial.print(F("] "));
            break;
            
        case String_t:
            print_string(variant.str, get_str_length());
            break;
            
        case Unsigned_t:
            Serial.print(this->variant.u);
            break;
        case Signed_t:
            Serial.print(this->variant.i);
            break;
        case Float_t:
            Serial.print(this->variant.number);
            break;
                    
        case Boolean_t:
            Serial.print((this->variant.truth ? F(" true ") : F(" false ")));
            break;
            
        case Null_t:
            Serial.print(F(" null "));
            break;
            
        case Function_t:
            Serial.print(F(" function "));
            break;
            
        case Proxy_t:
            Serial.print(F(" proxy "));
            break;
            
        case Thing_t:
            Serial.print(F(" thing "));
            break;
            
        case Unused_t:
            break;  // nothing to do
    }
}

void JSON::set_gc_phase(boolean phase)
{
    gc_phase = phase;
}

// mark this node and nodes reachable from it
void JSON::reachable(boolean phase)
{
    if (!marked(phase))
    {   
        switch (get_tag())
        {
            case Object_t:           
            case Array_t:
                AvlNode::apply(this->variant.object, (AvlApplyFn)reachable_item, (void *)phase);
                break;
                
            case Thing_t:
                this->variant.thing->reachable(phase);
                break;
                
            case Proxy_t:
                this->variant.proxy->reachable(phase);
                break;
            
            case Unused_t:
                break;

            default:
                toggle_mark();
                break;
        }
    }
}

void JSON::reachable_item(AvlKey key, AvlValue value, void *context)
{
    ((JSON *)value)->reachable((boolean)context);
}

// sweep this node and all nodes reachable from it that
// haven't already been marked as reachable from roots
void JSON::sweep(boolean phase)
{
    if (!marked(phase))
    {
        switch (get_tag())
        {
            case Object_t:           
            case Array_t:
                AvlNode::apply(this->variant.object, (AvlApplyFn)sweep_item, (void *)phase);
                AvlNode::free(this->variant.object);
                break;
                
            case Thing_t:
                this->variant.thing->sweep(phase);
                WebThings::remove_thing(this->variant.thing);
                break;
                
            case Proxy_t:
                this->variant.proxy->sweep(phase);
                WebThings::remove_proxy(this->variant.proxy);
                break;
            
            case Unused_t:
                break;

            default:
                break;
        }
        
        free();
    }
}


void JSON::sweep_item(AvlKey key, AvlValue value, void *context)
{
    ((JSON *)value)->sweep((boolean)context);
}

void JSON::free()
{
    Serial.print("freeing JSON node, tag "); Serial.println(get_tag());
    
    // safe against already freed node
    node_pool->free((wot_node_pool_t *)this);
}

// allocate node from fixed memory pool
JSON * JSON::new_node()
{
    JSON * node = (JSON *)(node_pool->allocate_node());
    
    if (node)
    {
        Serial.println("allocating JSON node");
        node->set_tag(Null_t);
        node->variant.number = 0.0;
        
        // set mark for this garbage collection cycle
        if (gc_phase)
            node->set_mark();
        else
            node->reset_mark();
    }
    
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

JSON * JSON::new_string(const __FlashStringHelper *str)
{
    return new_string(((char *)str)+PROGMEM_BOUNDARY);
}

JSON * JSON::new_string(char *str)
{
    return new_string(str, Strings::strlen(str));
}

JSON * JSON::new_string(char *str, unsigned int length)
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

// only JSON objects or arrays can have multiple
// references, so they need to be added the set
// of stale references, other nodes can be freed
boolean JSON::free_leaves()
{
    Json_Tag tag = get_tag();
    
    if (tag == Object_t || tag == Array_t)
        return 0;
    
    free();
    return 1;
}

Json_Tag JSON::get_tag()
{
    return (Json_Tag)(taglen & 0xF);
}

void JSON::set_tag(Json_Tag tag)
{
    taglen &= ~0xF;
    taglen |= (unsigned int)tag;
}

boolean JSON::marked(boolean phase)
{
    if (phase)
        return !(taglen & 0x10);
        
    return taglen & 0x10;
}

void JSON::toggle_mark()
{
    if (taglen & 0x10)
        taglen &= ~0x10;
    else
        taglen |= 0x10;
}

void JSON::set_mark()
{
    taglen |= 0x10;
}

void JSON::reset_mark()
{
    taglen &= ~0x10;
}

void JSON::set_obj_id(unsigned int id)
{
    taglen &= 0x1F;
    taglen |= (id << 5);
}

unsigned int JSON::get_obj_id()
{
    return taglen >> 5;
}

void JSON::set_str_length(unsigned int length)
{
    taglen &= 0x1F;
    taglen |= (length << 5);
}

unsigned int JSON::get_str_length()
{
    return taglen >> 5;
}

JSON * JSON::retrieve_property(Symbol symbol)
{
    AvlKey key = (AvlKey)symbol + 1;
    AvlNode::print_keys(variant.object);
    
    if (get_tag() == Object_t)
        return (JSON *)AvlNode::find_key(variant.object, key);
        
    return null;
}

void JSON::insert_property(Symbol symbol, JSON *new_value)
{
    if (get_tag() == Object_t) {
        AvlKey key = (AvlKey)symbol + 1;
        JSON *old_value =(JSON *)AvlNode::find_key(variant.object, key);
        variant.object = AvlNode::insert_key(variant.object, key, (void *)new_value);
        check_if_stale(old_value, new_value);
    }
}

GenericFn JSON::retrieve_function(Symbol action)
{
    if (get_tag() == Object_t) {
        JSON *func = (JSON *)AvlNode::find_key(variant.object, action);
        
        if (func && func->get_tag() == Function_t)
            return func->variant.function;
    }
    
    return null;
}

JSON * JSON::retrieve_array_item(unsigned int index)
{
    AvlKey key = (AvlKey)index + 1;

    if (get_tag() == Array_t)
        return (JSON *)AvlNode::find_key(variant.object, key);
        
    return null;
}

// Prepending is a lot harder and could be done
// by appending a copy of the last node value then
// shifting the values from one node to the next
// in a left to right traversal, and finally
// updating the first node. This will require a
// special helper in the AvlNode class.

void JSON::append_array_item(JSON *value)
{
    if (get_tag() == Array_t) {
        AvlKey last = AvlNode::last_key(variant.object);
        insert_array_item((unsigned int)last, value);
    }
}

void JSON::insert_array_item(unsigned int index, JSON *new_value)
{  
    if (get_tag() == Array_t) {
        AvlKey key = (AvlKey)index + 1;
        JSON * old_value = (JSON *)AvlNode::find_key(variant.object, key);
        variant.object =  AvlNode::insert_key(variant.object, key, (void *)new_value);
        check_if_stale(old_value, new_value);
    }
}

void JSON::check_if_stale(JSON * old_value, JSON * new_value)
{
    if (old_value && new_value != old_value)
        WebThings::add_stale(old_value);
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
    token_src = (char *)src;
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
            Strings::get_char(src+1) == 'u' &&
            Strings::get_char(src+2) == 'l' &&
            Strings::get_char(src+3) == 'l')
        {
            src += 4;
            length += 4;
            return Null_token;
        }
    }
    else if (c == 't')
    {
        if (length >= 4 &&
            Strings::get_char(src+1) == 'r' &&
            Strings::get_char(src+2) == 'u' &&
            Strings::get_char(src+3) == 'e')
        {
            src += 4;
            length += 4;
            return True_token;
        }
    }
    else if (c == 'f')
    {
        if (length >= 5 &&
            Strings::get_char(src+1) == 'a' &&
            Strings::get_char(src+2) == 'l' &&
            Strings::get_char(src+3) == 's' &&
            Strings::get_char(src+4) == 'e')
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
    token_src = (char *)src;
    token_len = 0;
    
    while (length > 0)
    {
        --length;
        
        if (Strings::get_char(src++) == '"')
            return String_token;
        
        ++token_len;
    }
    
    Serial.println(F("Missing trailing quote mark"));
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
        return Strings::get_char(src);
    
    return 256;
}
