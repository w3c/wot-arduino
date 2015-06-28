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
        static string stringify(JSON& obj);
        static JSON * parse(string src);
        
        JSON ();
        
    private:
        void to_string(string &str);
        
        class JsonObj
        {
            AvlNode *root;  
              
            void insert(string &name, JSON& value);
            JSON *retrieve(string &name);
            
            JsonObj ()
            {
                root = null;
            }
        };
        
        Json_Tag tag;
        
        union js_union
        {
            string *str;
            int i;
            float x;
            JsonObj *obj;
        } variant;
};
