/* Minimal JSON support for Arduino */

#include <ctype.h>
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

JSON * JSON::parse(const char *src, unsigned int length)
{
    Lexer lexer;
    lexer.src = (unsigned char *)src;
    lexer.length = length;
    cout << "parsing " << src << "\n";
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
            cout << "JSON syntax error\n";
            return null;
    }
    
    token = lexer->get_token();
    
    if (token != Error_token)
    {
        cout << "JSON syntax error\n";
        return null;
    }

    return null;
}

JSON * JSON::parse_object(Lexer *lexer)
{
    Json_Token token = lexer->get_token();
    
    while (token != Error_token && token != Object_stop_token)
    {
        token = lexer->get_token();
        
        if (token == Object_start_token)
            parse_object(lexer);
        else if (token == Array_start_token)
            parse_array(lexer);
    }
    
    return null;
}

JSON * JSON::parse_array(Lexer *lexer)
{
    Json_Token token = lexer->get_token();
    
    while (token != Error_token && token != Array_stop_token)
    {
        token = lexer->get_token();
        
        if (token == Object_start_token)
            parse_object(lexer);
        else if (token == Array_start_token)
            parse_array(lexer);
    }
    
    return null;
}

void JSON::print()
{
    switch (this->tag)
    {
        case Object_t:
            cout << " object ";
            break;
            
        case Array_t:
            cout << " array ";
            break;
            
        case String_t:
            cout << " string ";
            break;
            
        case Unsigned_t:
        case Signed_t:
        case Float_t:
            cout << " number ";
            break;
                    
        case Boolean_t:
            cout << (this->variant.truth ? " true " : " false ");
        case Null_t:
            cout << " null ";
            break;
    }
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

JSON * JSON::new_string(unsigned char *str, unsigned int length)
{
    JSON *node = JSON::new_node();
    
    if (node)
    {
        node->tag = String_t;
        node->variant.str = str;
        node->token_len = length;
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
            next_byte();
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
