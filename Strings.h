// support for strings in RAM and program memory on the AVR processors
// this merges the address space by using an offset of #8000

#ifndef _WOTF_STRINGS
#define _WOTF_STRINGS

#define PROGMEM_BOUNDARY 0x8000

class Strings
{
    public:
    
    static char get_char(const char *p);
    static void print(const char *s, unsigned int len);
    static unsigned int strlen(const char *p);
    static int strcmp(const char *s1, const char *s2);
    static int strcmp(const char *s1, unsigned int len1,
                      const char *s2, unsigned int len2);
    static char *strcpy(char *dst, const char *src);
};
#endif