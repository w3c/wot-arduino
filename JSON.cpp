/* Minimal JSON support for Arduino */

#include <iostream>
using namespace std;

#include "JSON.h"

/*
    public:
        static string stringify(JSON& obj);
        static JSON * parse(string src);
        JSON ()
        {
            memset(this, sizeof(JSON), 0);
        }
        
    private:
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
*/

string JSON::stringify(JSON& obj)
{
    return "hello world";
}

JSON * JSON::parse(string src)
{
    return (JSON *)0;
}

void JSON::JsonObj::insert(string &name, JSON& value)
{
}

JSON * JSON::JsonObj::retrieve(string &name)
{
    return (JSON *)0;
}
