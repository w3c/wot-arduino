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
*/

#include <iostream>
using namespace std;

#include "MessageCoder.h"

void MessageBuffer::set_buffer(unsigned char *buf, unsigned len)
{
    union unum {
        unsigned char bytes[4];
        uint16_t u;
        int16_t i;
        float x;
    } num;

    buffer = buf;
    length = len;
    size = index = 0;
    
    num.u = 1;
    big_endian = (num.bytes[0] == 1 ? true : false);
}

bool MessageBuffer::is_big_endian()
{
    return big_endian;
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

bool MessageBuffer::put_byte(unsigned char c)
{
    if (index < length)
    {
        buffer[size++] = c & 255;
        return true;
    }
    
    cout << "attempt to write beyond end of buffer";
    return false;
}

void MessageCoder::decode_object(MessageBuffer *buffer)
{
    unsigned int c;
    String s;
    
    cout << "start object\n";
    
    for (;;)
    {
        // get name
        c = buffer->get_byte();
        
        if (c ==  WOT_STRING)
        {
            s = buffer->get_pointer();
            while ((c = buffer->get_byte()) && c < 256); 
            cout << "string \""; cout << c; cout << "\" :\n";
        
            // get value
            decode(buffer);
        }
        else if (WOT_SYM_BASE <= c && c < 256)
        {
            c = c - WOT_SYM_BASE;
            cout << "symbol "; cout << c; cout << " :\n";
        
            // get value
            decode(buffer);
        }
        else if (c == WOT_END_OBJECT)
        {
            break;
        }
        else
        {
            cout << "didn't find string or symbol for object property name";
            break;
        }
    }
    
    cout << "end object\n";
}

void MessageCoder::decode_array(MessageBuffer *buffer)
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
            break;
        }
            
        decode(buffer);
    }
    
    cout << "end array\n";
}

void MessageCoder::decode(MessageBuffer *buffer)
{
    unsigned int u, c;
    int i;
    float x;
    String s;
    
    union unum {
        unsigned char bytes4[4];
        unsigned char bytes2[4];
        uint16_t u;
        int16_t i;
        float x;
    } num;
            
    c = buffer->get_byte();
    
    if (c == WOT_START_OBJECT)
    {
        decode_object(buffer);
    }
    else if (c == WOT_START_ARRAY)
    {
        decode_array(buffer);
    }
    else if (c == WOT_STRING)
    {
        s = buffer->get_pointer();
        while ((c = buffer->get_byte()) && c < 256); 
        cout << "string \"" << s << "\"\n";
    }
    else if (c == WOT_UNSIGNED_INT_8)
    {
        c = buffer->get_byte();
        cout << "unsigned integer "; cout << c; cout << "\n";
    }
    else if (c == WOT_UNSIGNED_INT_16)
    {
        unsigned char c1 = buffer->get_byte();
        unsigned char c2 = buffer->get_byte();

        u = (c1 << 8) | c2;
        cout << "unsigned integer "; cout << u; cout << "\n";
    }
    else if (c == WOT_SIGNED_INT_8)
    {
        u = (c << 8) | buffer->get_byte();
        i = (uint16_t)c;
        cout << "signed integer "; cout << i; cout << "\n";
    }
    else if (c == WOT_SIGNED_INT_16)
    {
        unsigned char c1 = buffer->get_byte();
        unsigned char c2 = buffer->get_byte();
        u = (c1 << 8) | c2;
        i = (int16_t)u;
        cout << "signed integer "; cout << i; cout << "\n";
    }
    else if (c == WOT_FLOAT_32)
    {
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
        
        x = num.x;
        cout << "float "; cout << x; cout << "\n";
    }
    else if (WOT_RESERVED_START <= c && c <= WOT_RESRVED_END)
    {
        cout << "illegal use of reserved tag";
    }
    else if (WOT_NUM_BASE <= c && c < WOT_SYM_BASE)
    {
        u = c - WOT_NUM_BASE;
        cout << "unsigned integer "; cout << u; cout << "\n";

    }
    else if (WOT_SYM_BASE <= c && c < 256)
    {
        u = c - WOT_SYM_BASE;
        cout << "symbol "; cout << u; cout << "\n";
    }
    else // unexpected end of buffer
    {
        cout << "unexpectedly reached end of buffer\n";
    }
}

void MessageCoder::encode_unsigned(MessageBuffer *buffer, unsigned int n)
{
    if (n < (WOT_SYM_BASE - WOT_NUM_BASE))
        buffer->put_byte(n + WOT_NUM_BASE);
    else if (n < 256)
    {
        buffer->put_byte(WOT_UNSIGNED_INT_8);
        buffer->put_byte(n);
    }
    else  // for now assume fits in 16 bits
    {
        uint16_t u = (uint16_t) n;
        
        buffer->put_byte(WOT_UNSIGNED_INT_16);
        buffer->put_byte(u >> 8);
        buffer->put_byte(u & 255);
    }
}

void MessageCoder::encode_signed(MessageBuffer *buffer, int n)
{
    if (0 <= n && n < (WOT_SYM_BASE - WOT_NUM_BASE))
        buffer->put_byte(n + WOT_NUM_BASE);
    else if (-128 < n && n <= 127)
    {
        buffer->put_byte(WOT_SIGNED_INT_8);
        buffer->put_byte((unsigned char)n);
    }
    else  // for now assume fits in 16 bits
    {
        uint16_t u = (uint16_t)n;
        buffer->put_byte(WOT_SIGNED_INT_16);
        buffer->put_byte(u >> 8);
        buffer->put_byte(u & 255);
    }
}

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

void MessageCoder::encode_string(MessageBuffer *buffer, String str)
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
    
    cout << "float has " << sizeof(float) << " bytes\n";
    
    membuf.set_buffer(&buffer[0], WOT_MESSAGE_LENGTH);
    coder.encode_string(&membuf, (String)"hello world");
    cout << "used " << membuf.get_size() << " bytes\n";
    coder.decode(&membuf);
    
    membuf.set_buffer(&buffer[0], WOT_MESSAGE_LENGTH);
    coder.encode_array_start(&membuf);
    coder.encode_string(&membuf, (String)"one");
    coder.encode_string(&membuf, (String)"two");
    coder.encode_string(&membuf, (String)"three");
    coder.encode_symbol(&membuf, 79);
    coder.encode_unsigned(&membuf, 7);
    coder.encode_unsigned(&membuf, 24);
    coder.encode_unsigned(&membuf, 200);
    coder.encode_unsigned(&membuf, 300);
    coder.encode_signed(&membuf, -320);
    coder.encode_float(&membuf, 1.414259);
    coder.encode_array_end(&membuf);
    cout << "used " << membuf.get_size() << " bytes\n";
    coder.decode(&membuf);
    
}