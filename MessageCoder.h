#ifndef _WOTF_MESSAGECODER
#define _WOTF_MESSAGECODER

// unsigned char tag codes

#define WOT_END_OBJECT 0
#define WOT_START_OBJECT 1
#define WOT_END_ARRAY 2
#define WOT_START_ARRAY 3
#define WOT_STRING 4
#define WOT_UNSIGNED_INT_8 5
#define WOT_UNSIGNED_INT_16 6
#define WOT_UNSIGNED_INT_32 7
#define WOT_SIGNED_INT_8 8
#define WOT_SIGNED_INT_16 9
#define WOT_SIGNED_INT_32 10
#define WOT_FLOAT_32 11
#define WOT_VALUE_TRUE 12
#define WOT_VALUE_FALSE 13
#define WOT_VALUE_NULL 14

// used to signal decoding overrun
#define WOT_BUFFER_EMPTY 256

// tags in range 15-22 are reserved for future use

#define WOT_RESERVED_START 15
#define WOT_RESRVED_END 22

// tags in range 23-54 are integers (0-31)

#define WOT_NUM_BASE 23

// tags in range 55-255 are symbols (1 through 200)

#define WOT_SYM_BASE 55

class MessageBuffer
{
    private:
        unsigned char *buffer;
        unsigned int length;
        unsigned int index;
        unsigned int size;
        int big_endian;
        bool overflow;
        
    public:
        boolean is_big_endian();
        void set_buffer(unsigned char *buf, unsigned len);
        void reset();
        void restart();
        unsigned char * get_pointer();
        boolean overflowed();
        unsigned int get_size();
        unsigned int get_byte();
        unsigned int view_byte();
        unsigned int remaining();
        bool put_byte(unsigned char c);
};

class MessageCoder
{
    private:
        static boolean decode_value(MessageBuffer *buffer);
        static boolean decode_object(MessageBuffer *buffer);
        static boolean decode_array(MessageBuffer *buffer);
    
    public:
        //static void test();
        
        static boolean decode(MessageBuffer *buffer);
        static void encode_unsigned8(MessageBuffer *buffer, unsigned char n);
        static void encode_unsigned16(MessageBuffer *buffer, uint16_t n);
        static void encode_unsigned32(MessageBuffer *buffer, uint32_t n);
        static void encode_signed8(MessageBuffer *buffer, char n);
        static void encode_signed16(MessageBuffer *buffer, int16_t n);
        static void encode_signed32(MessageBuffer *buffer, int32_t n);
        static void encode_float(MessageBuffer *buffer, float x);
        static void encode_null(MessageBuffer *buffer);
        static void encode_true(MessageBuffer *buffer);
        static void encode_false(MessageBuffer *buffer);
        static void encode_symbol(MessageBuffer *buffer, unsigned int sym);
        static void encode_string(MessageBuffer *buffer, unsigned char *str);
#if defined(pgm_read_byte)
        static void encode_string(MessageBuffer *buffer, const __FlashStringHelper *str);
#endif
        static void encode_object_start(MessageBuffer *buffer);
        static void encode_object_end(MessageBuffer *buffer);
        static void encode_array_start(MessageBuffer *buffer);
        static void encode_array_end(MessageBuffer *buffer);
};

#endif