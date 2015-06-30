/* Minimal JSON support for Arduino */

#include "core.h"
#include "AvlNode.h"

using namespace std;  // for string and memset

enum Json_Tag { Array_t, Object_t, String_t, Number_t, Boolean_t, Null_t };

class JSON
{
    public:
        static void initialise_pool(JSON *pool, unsigned int size);
        static string stringify(JSON& obj);
        static JSON * parse(String src);

        static JSON * newNumber(float x);
        static JSON * newNull();
        static JSON * newBoolean(bool value);
        static JSON * newString(String str);
        static JSON * newObject();
        static JSON * newArray(unsigned int size);
        
    private:
        static unsigned int length;
        static unsigned int size;
        static JSON *pool;
        
        static JSON * newNode();

        void to_string(String str);
        void insert(String name, JSON *value);
        JSON *retrieve(String name);
        
        Json_Tag tag;
        
        union js_union
        {
            String str;
            float number;
            int truth;
            JSON *array;
            AvlNode *object;
        } variant;
};
