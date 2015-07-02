/*
    Experimental WoT Message encoder/decoder
   
    A server hosting a proxy for a thing, and the server hosting the
    thing it proxies both have shared access to the things data model.
    This can be exploited for concise message encodings by sending a
    symbol in place of a string. For common situations, a symbol can be
    encoded with a singe byte.
   
    The message format starts with an identifier for the thing. This is
    followed by an item. Each item is at least one byte in length and
    is one of: symbol, number, string, or object. Objects are formed by
    zero or more pairs of name/value pairs followed by a byte code that
    marks the end of the object. Names are symbols or strings. Values are
    items. Strings are null terminated UTF-8 byte sequences. Numbers are
    signed or unsigned integers and single precision floats. Integers
    take one or more bytes to encode.
   
    Symbols include a core set that are used in messages, specials such
    as true, false, and null, and thing specific symbols. A single byte
    is used for the first 200 symbols, which should be sufficient in
    most circumstances for microcontroller based servers.
   
    Open questions:
   
    For microcontrollers with low memory and small packet sizes,
    we just need to support a bare minimum of core data types.
    
    If we want to expand the scope to very high through-put servers,
    then a more extensive set of core data types could be valuable. 

       Do we need 64 bit precision floats for IoT applications?
       Do we need 64 bit timestamps (mS since epoch)?
       Do we need to distinguish URIs from strings?
       Do we need an arbitrary length bit vector?
       If we need binary blobs, how to encode their content type?
       
    The answer is no for all of these, at least for now. There are
    plenty of reserved tags we could use for extensions.
       
    The Arduino treats both float and double as 32 bit IEEE floating
    point numbers, and also lacks support for 64 bit integers. There
    is a library for getting the time in seconds since epoch. Finer
    grained time will be needed for machine control applications.
*/

#include <iostream>
using namespace std;

#include "MessageCoder.h"

void MessageBuffer::set_buffer(unsigned char *buf, unsigned len)
{
    union unum {
        unsigned char bytes[2];
        uint16_t u;
    } num;

    buffer = buf;
    length = len;
    size = index = 0;
    overflow = false;
    
    num.u = 1;
    big_endian = (num.bytes[0] == 1 ? true : false);
}

boolean MessageBuffer::is_big_endian()
{
    return big_endian;
}

boolean MessageBuffer::overflowed()
{
    return overflow;
}

unsigned char * MessageBuffer::get_pointer()
{
    return buffer + index;
}

unsigned int MessageBuffer::get_size()
{
    return size;
}

unsigned int MessageBuffer::get_byte()
{
    if (index < length)
        return buffer[index++]; 
        
    return 256;  // to signal error
}

unsigned int MessageBuffer::view_byte()
{
    if (index < length)
        return buffer[index]; 
        
    return 256;  // to signal error
}

boolean MessageBuffer::put_byte(unsigned char c)
{
    if (size < length)
    {
        buffer[size++] = c & 255;
        return true;
    }

    overflow = true;    
    return false;
}

boolean MessageCoder::decode_object(MessageBuffer *buffer)
{
    unsigned int c;
    unsigned char *s;
    
    cout << "start object\n";
    
    for (;;)
    {
        // get name
        c = buffer->get_byte();
        
        if (c ==  WOT_STRING)
        {
            s = buffer->get_pointer();
            while ((c = buffer->get_byte()) && c < 256);
            
            if (c)
            {
                cout << "unterminated string\n";
                return false;
            }
            else 
                cout << "string \""; cout << s; cout << "\" :\n";
        
            // get value
            
            if (!decode(buffer))
                return false;
        }
        else if (WOT_SYM_BASE <= c && c < 256)
        {
            c = c - WOT_SYM_BASE;
            cout << "symbol "; cout << c; cout << " :\n";
        
            // get value
            
            if (!decode(buffer))
                return false;
        }
        else if (c == WOT_END_OBJECT)
        {
            break;
        }
        else
        {
            cout << "didn't find string or symbol for object property name";
            return false;
        }
    }
    
    cout << "end object\n";
    return true;
}

boolean MessageCoder::decode_array(MessageBuffer *buffer)
{
    unsigned int c;
    
    cout << "start array\n";
    
    for (;;)
    {
        c = buffer->view_byte();
        
        if (c ==  WOT_END_ARRAY)
            break;
            
        if (c == WOT_END_OBJECT)
        {
            cout << "found unexpected end of object\n";
            return false;
        }
            
        if (!decode(buffer))
            return false;
    }
    
    cout << "end array\n";
    return true;
}

boolean MessageCoder::decode(MessageBuffer *buffer)
{
    unsigned int c = buffer->get_byte();
    
    switch (c)
    {
        case WOT_START_OBJECT:
            return decode_object(buffer);
            
        case WOT_START_ARRAY:
            return decode_array(buffer);
    
        case WOT_STRING:
        {
            unsigned char *s = buffer->get_pointer();
            while ((c = buffer->get_byte()) && c < 256); 
            
            if (c)
            {
                cout << "unterminated string\n";
                return false;
            }
            else
                cout << "string \"" << s << "\"\n";
            break;
        }

        case WOT_UNSIGNED_INT_8:
            c = buffer->get_byte();
            cout << "unsigned 8 bit integer " << c << "\n";
            break;
    
        case WOT_SIGNED_INT_8:
        {
            c = buffer->get_byte();
            uint16_t i = (uint16_t)c;
            cout << "signed 8 bit integer " << i << "\n";
            break;
        }
        
        case WOT_UNSIGNED_INT_16:
        case WOT_SIGNED_INT_16:
        {
            union unum
            {
                unsigned char bytes2[2];
                uint16_t u;
                int16_t i;
            } num;

            if (buffer->is_big_endian())
            { 
                num.bytes2[1] = buffer->get_byte();
                num.bytes2[0] = buffer->get_byte();
            }
            else
            {
                num.bytes2[0] = buffer->get_byte();
                num.bytes2[1] = buffer->get_byte();
            }
            
            if (c == WOT_UNSIGNED_INT_16)
                cout << "unsigned 16 bit integer " << num.u << "\n";
            else
                cout << "signed 16 bit integer " << num.i << "\n";
            break;
        }
    
        case WOT_UNSIGNED_INT_32:
        case WOT_SIGNED_INT_32:
        case WOT_FLOAT_32:
        {
            union unum
            {
                unsigned char bytes4[4];
                uint32_t u;
                int32_t i;
                float x;
            } num;

            if (buffer->is_big_endian())
            { 
                num.bytes4[3] = buffer->get_byte();
                num.bytes4[2] = buffer->get_byte();
                num.bytes4[1] = buffer->get_byte();
                num.bytes4[0] = buffer->get_byte();
            }
            else
            {
                num.bytes4[0] = buffer->get_byte();
                num.bytes4[1] = buffer->get_byte();
                num.bytes4[2] = buffer->get_byte();
                num.bytes4[3] = buffer->get_byte();
            }

            if (c == WOT_UNSIGNED_INT_32)
                cout << "unsigned 32 bit integer " << num.u << "\n";
            else if (c == WOT_SIGNED_INT_32)
                cout << "signed 32 bit integer " << num.i << "\n";
            else
            {
                cout.precision(7);
                cout << "float " << num.x << "\n";
            }
            break;
        }
        
        case WOT_VALUE_NULL:
            cout << "null\n";
            break;
        
        case WOT_VALUE_TRUE:
            cout << "true\n";
            break;
        
        case WOT_VALUE_FALSE:
            cout << "false\n";
            break;
        
        default:
        {
            if (WOT_RESERVED_START <= c && c <= WOT_RESRVED_END)
            {
                cout << "illegal use of reserved tag";
                return false;
            }
            else if (WOT_NUM_BASE <= c && c < WOT_SYM_BASE)
            {
                uint16_t u = c - WOT_NUM_BASE;
                cout << "unsigned 4 bit integer " << u << "\n";

            }
            else if (WOT_SYM_BASE <= c && c < 256)
            {
                c -= WOT_SYM_BASE;
                cout << "symbol " << c << "\n";
            }
            else // unexpected end of buffer
            {
                cout << "unexpectedly reached end of buffer\n";
                return false;
            }
        }
    }
    
    return true;
}

void MessageCoder::encode_unsigned8(MessageBuffer *buffer, unsigned char n)
{
    if (n < (WOT_SYM_BASE - WOT_NUM_BASE))
        buffer->put_byte(n + WOT_NUM_BASE);
    else
    {
        buffer->put_byte(WOT_UNSIGNED_INT_8);
        buffer->put_byte(n);
    }
}

void MessageCoder::encode_unsigned16(MessageBuffer *buffer, uint16_t n)
{
    if (n < (WOT_SYM_BASE - WOT_NUM_BASE))
        buffer->put_byte(n + WOT_NUM_BASE);
    else if (n < 256)
    {
        buffer->put_byte(WOT_UNSIGNED_INT_8);
        buffer->put_byte(n);
    }
    else
    {
        uint16_t u = (uint16_t) n;
        
        buffer->put_byte(WOT_UNSIGNED_INT_16);
        buffer->put_byte(u >> 8);
        buffer->put_byte(u & 255);
    }
}

void MessageCoder::encode_unsigned32(MessageBuffer *buffer, uint32_t n)
{
    if (n < (WOT_SYM_BASE - WOT_NUM_BASE))
        buffer->put_byte(n + WOT_NUM_BASE);
    else if (n < 256)
    {
        buffer->put_byte(WOT_UNSIGNED_INT_8);
        buffer->put_byte(n);
    }
    else if (n < 65536)
    {
        uint16_t u = (uint16_t) n;
        
        buffer->put_byte(WOT_UNSIGNED_INT_16);
        buffer->put_byte(u >> 8);
        buffer->put_byte(u & 255);
    }
    else // assume 32 bit unsigned int
    {
        uint32_t u = (uint32_t) n;
        
        buffer->put_byte(WOT_UNSIGNED_INT_32);
        buffer->put_byte((u >> 24) & 255);
        buffer->put_byte((u >> 1) & 255);
        buffer->put_byte((u >> 8) & 255);
        buffer->put_byte(u & 255);
    }
}

void MessageCoder::encode_signed8(MessageBuffer *buffer, char n)
{
    if (n < (WOT_SYM_BASE - WOT_NUM_BASE))
        buffer->put_byte((unsigned char)n + WOT_NUM_BASE);
    else
    {
        buffer->put_byte(WOT_SIGNED_INT_8);
        buffer->put_byte((unsigned char)n);
    }
}

void MessageCoder::encode_signed16(MessageBuffer *buffer, int16_t n)
{
    if (0 <= n && n < (WOT_SYM_BASE - WOT_NUM_BASE))
        buffer->put_byte(n + WOT_NUM_BASE);
    else if (-128 < n && n <= 127)
    {
        buffer->put_byte(WOT_SIGNED_INT_8);
        buffer->put_byte((unsigned char)n);
    }
    else
    {
        uint16_t u = (uint16_t) n;
        
        buffer->put_byte(WOT_SIGNED_INT_16);
        buffer->put_byte(u >> 8);
        buffer->put_byte(u & 255);
    }
}

void MessageCoder::encode_signed32(MessageBuffer *buffer, int32_t n)
{
    if (0 <= n && n < (WOT_SYM_BASE - WOT_NUM_BASE))
        buffer->put_byte(n + WOT_NUM_BASE);
    else if (-128 < n && n <= 127)
    {
        buffer->put_byte(WOT_SIGNED_INT_8);
        buffer->put_byte((unsigned char)n);
    }
    else if (n < 32768)
    {
        uint16_t u = (uint16_t) n;        
        buffer->put_byte(WOT_SIGNED_INT_16);
        buffer->put_byte(u >> 8);
        buffer->put_byte(u & 255);
    }
    else
    {
        uint32_t u = (uint32_t) n;        
        buffer->put_byte(WOT_SIGNED_INT_32);
        buffer->put_byte((u >> 24) & 255);
        buffer->put_byte((u >> 1) & 255);
        buffer->put_byte((u >> 8) & 255);
        buffer->put_byte(u & 255);
    }
}

// Arduino uses 32 bits for both float and double
void MessageCoder::encode_float(MessageBuffer *buffer, float x)
{
    buffer->put_byte(WOT_FLOAT_32);
    
    union unum {
        unsigned char bytes4[4];
            float x;
        } num;
        
        num.x = x;

    if (buffer->is_big_endian())
    {
        buffer->put_byte(num.bytes4[3]);
        buffer->put_byte(num.bytes4[2]);
        buffer->put_byte(num.bytes4[1]);
        buffer->put_byte(num.bytes4[0]);
    }
    else
    { 
        buffer->put_byte(num.bytes4[0]);
        buffer->put_byte(num.bytes4[1]);
        buffer->put_byte(num.bytes4[2]);
        buffer->put_byte(num.bytes4[3]);
    }
}

void MessageCoder::encode_symbol(MessageBuffer *buffer, unsigned int sym)
{
    if (sym > 200)
    {
        cout << "symbol out of range";   
    }
    else
    {
        buffer->put_byte(sym + WOT_SYM_BASE);
    }
}

void MessageCoder::encode_null(MessageBuffer *buffer)
{
    buffer->put_byte(WOT_VALUE_NULL);
}

void MessageCoder::encode_true(MessageBuffer *buffer)
{
    buffer->put_byte(WOT_VALUE_TRUE);
}

void MessageCoder::encode_false(MessageBuffer *buffer)
{
    buffer->put_byte(WOT_VALUE_FALSE);
}

void MessageCoder::encode_string(MessageBuffer *buffer, unsigned char *str)
{
    unsigned char c, *p = str;
    
    buffer->put_byte(WOT_STRING);
    
    while ((c = *p++))
        buffer->put_byte(c);
        
    buffer->put_byte(0);
}

void MessageCoder::encode_object_start(MessageBuffer *buffer)
{
    buffer->put_byte(WOT_START_OBJECT);
}

void MessageCoder::encode_object_end(MessageBuffer *buffer)
{
    buffer->put_byte(WOT_END_OBJECT);
}

void MessageCoder::encode_array_start(MessageBuffer *buffer)
{
    buffer->put_byte(WOT_START_ARRAY);
}

void MessageCoder::encode_array_end(MessageBuffer *buffer)
{
    buffer->put_byte(WOT_END_ARRAY);
}

#define WOT_MESSAGE_LENGTH 128

void MessageCoder::test()
{
    MessageBuffer membuf;
    MessageCoder coder;
    
    unsigned char buffer[WOT_MESSAGE_LENGTH];
    const float PI = 3.1415926;
    
    cout << "int has " << sizeof(int) << " bytes\n";
    cout << "long has " << sizeof(long) << " bytes\n";
    cout << "float has " << sizeof(float) << " bytes\n";
    cout << "double has " << sizeof(double) << " bytes\n";
    
    membuf.set_buffer(&buffer[0], WOT_MESSAGE_LENGTH);

    coder.encode_string(&membuf, (unsigned char *)"hello world");
    cout << "used " << membuf.get_size() << " bytes\n";
    cout << "overflowed: " << (membuf.overflowed() ? "true" : "false") << "\n";

    coder.decode(&membuf);
    
    membuf.set_buffer(&buffer[0], WOT_MESSAGE_LENGTH);

    coder.encode_array_start(&membuf);
    coder.encode_string(&membuf, (unsigned char *)"one");
    coder.encode_string(&membuf, (unsigned char *)"two");
    coder.encode_string(&membuf, (unsigned char *)"three");
    coder.encode_symbol(&membuf, 79);
    coder.encode_null(&membuf);
    coder.encode_true(&membuf);
    coder.encode_false(&membuf);
    coder.encode_unsigned16(&membuf, 7);
    coder.encode_unsigned16(&membuf, 24);
    coder.encode_unsigned16(&membuf, 200);
    coder.encode_unsigned16(&membuf, 300);
    coder.encode_unsigned32(&membuf, 65536);
    coder.encode_signed16(&membuf, -320);
    coder.encode_signed16(&membuf, -32768);
    coder.encode_signed32(&membuf, 32768);
    coder.encode_float(&membuf, (float)PI);
    coder.encode_array_end(&membuf);
    
    cout << "used " << membuf.get_size() << " bytes\n";
    cout << "overflowed: " << (membuf.overflowed() ? "true" : "false") << "\n";

    coder.decode(&membuf);
    
}