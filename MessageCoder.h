#include <stdint.h>
#include "core.h"

// unsigned char tag codes

#define WOT_END_OBJECT 0
#define WOT_START_OBJECT 1
#define WOT_END_ARRAY 2
#define WOT_START_ARRAY 3
#define WOT_STRING 4
#define WOT_UNSIGNED_INT_8 5
#define WOT_UNSIGNED_INT_16 6
#define WOT_SIGNED_INT_8 7
#define WOT_SIGNED_INT_16 8
#define WOT_FLOAT_32 9

// tags in range 10-22 are reserved for future use

#define WOT_RESERVED_START 10
#define WOT_RESRVED_END 22

// tags in range 23-54 are integers (0-31)

#define WOT_NUM_BASE 23

// tags in range 55-255 are symbols (0 through 200)

#define WOT_SYM_BASE 55

// some predefined symbols

#define WOT_SYM_NULL 0
#define WOT_SYM_TRUE 1
#define WOT_SYM_FALSE 2

class MessageBuffer
{
    private:
        unsigned char *buffer;
        unsigned int length;
        unsigned int index;
        unsigned int size;
        int big_endian;
        
    public:
        bool is_big_endian();
        void set_buffer(unsigned char *buf, unsigned len);
        unsigned char * get_pointer();
        unsigned int get_size();
        unsigned int get_byte();
        unsigned int view_byte();
        bool put_byte(unsigned char c);
};

class MessageCoder
{
    public:
        static void test();
          
        static void decode(MessageBuffer *buffer);
        static void decode_object(MessageBuffer *buffer);
        static void decode_array(MessageBuffer *buffer);

        static void encode_unsigned(MessageBuffer *buffer, unsigned int n);
        static void encode_signed(MessageBuffer *buffer, int n);
        static void encode_float(MessageBuffer *buffer, float x);
        static void encode_symbol(MessageBuffer *buffer, unsigned int sym);
        static void encode_string(MessageBuffer *buffer, String str);
        static void encode_object_start(MessageBuffer *buffer);
        static void encode_object_end(MessageBuffer *buffer);
        static void encode_array_start(MessageBuffer *buffer);
        static void encode_array_end(MessageBuffer *buffer);
};

