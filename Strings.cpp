// some common operations on strings that are designed to work
// whether the strings are stored in AVR RAM or program memory

#include <Arduino.h>
#include "Strings.h"

char Strings::get_char(const char *p)
{
#if defined(pgm_read_byte)
    if ((unsigned int)p >= PROGMEM_BOUNDARY) {
        return (char)pgm_read_byte(p - PROGMEM_BOUNDARY);
    } else
        return *p;
#else
    return *p;
#endif
}

unsigned int Strings::strlen(const char *s)
{
    unsigned int len = 0;
    
    while (get_char(s++))
        ++len;
        
    return len;
}

void Strings::print(const char *s, unsigned int len)
{
    Serial.print(len);
    Serial.print(F(" \""));
    while (len--)
        Serial.print(get_char(s++));
    Serial.print(F("\""));
}

// for null terminated strings
int Strings::strcmp(const char *s1, const char *s2)
{
    char c1, c2;
    
    while ((c1 = get_char(s1)) == (c2 = get_char(s2)))
    {
        if (!c1)
            return 0;
            
        s1++;
        s2++;
    }
    
    return ((unsigned)c1 - (unsigned)c2);
}

// for strings with known length
int Strings::strcmp(const char *s1, unsigned int len1,
                    const char *s2, unsigned int len2)
{
    if (len1 && len2)
    {
        while(--len1 && --len2 && (get_char(s1)==get_char(s2)))
        {
            s1++;
            s2++;
        }
        
        return ((unsigned)get_char(s1)-(unsigned)get_char(s2));
    }
        
    if (!(len1 | len2))
        return 0;

    return (len1 ? 1 : -1);
}

char *Strings::strcpy(char *dst, const char *src)
{
    while ((*dst = get_char(src++)))
        ++dst;
        
    return dst;
}
