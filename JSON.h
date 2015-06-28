/* Minimal JSON support for Arduino */

#include <string>
#include "AvlNode.h"

using namespace std;  // for string and memset

enum Json_Tag { Array, Object, String, Number, Boolean, Null };

#define null 0
#define false 0
#define true  (!false)


class JSON
{
    public:
        static void initialise_pool(JSON *pool, unsigned int size);
        static string stringify(JSON& obj);
        static JSON * parse(string src);

        static JSON * newNumber(float x);
        static JSON * newNull();
        static JSON * newBoolean(bool value);
        static JSON * newString(string& str);
        static JSON * newObject();
        static JSON * newArray(unsigned int size);
        
    private:
        static unsigned int length;
        static unsigned int size;
        static JSON *pool;
        
        static JSON * newNode();

        void to_string(string& str);
        void insert(string &name, JSON *value);
        JSON *retrieve(string &name);
        
        Json_Tag tag;
        
        union js_union
        {
            string *str;
            float number;
            int truth;
            JSON *array;
            AvlNode *object;
        } variant;
};
