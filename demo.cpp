/* demo code for Web of Things Framework for Arduino */

#include <iostream>

using namespace std;

#include "WebThings.h"
#include "JSON.h"
#include "MessageCoder.h"

int main ()
{
    WebThings wot;
    
    cout << "started server\n\n";
    
    const char *agent_model =
        "{"
            "\"@properties\": {"
                "\"door\": {"
                    "\"type\":  \"thing\","
                    "\"uri\":  \"door12\""
                "},"
                "\"light\": {"
                    "\"type\":  \"thing\","
                    "\"uri\":  \"light12\""
                "}"
            "},"
        "}";

    const char *door_model =
        "{"
            "\"@events\": {"
                "\"bell\": null,"
                "\"key\": {"
                    "\"valid\": \"boolean\""
                "}"
            "},"
            "\"@properties\": {"
                "\"is_open\": \"boolean\""
            "},"
            "\"@actions\": {"
                "\"unlock\": null"
            "}"
        "}";
    
    const char *light_model =
        "{"
            "\"@properties\": {"
                "\"on\": {"
                    "\"type\":  \"boolean\","
                    "\"writeable\":  true"
                "}"
            "},"
        "}";
    
    Thing *agent = wot.thing("agent12", agent_model);
    Thing *door = wot.thing("door12", door_model);
    Thing *light = wot.thing("light12", light_model);
    
    //cout << door_model;
    
    //MessageCoder coder;
    //coder.test();
    
    const char *test = "[ \"hello\", [null]]";
    
    JSON *json = JSON::parse(test);
    
    cout << "parsing complete\n";
    
    if (json)
        json->print();
    else
        cout << "undefined";
        
    cout << "\n";
    
    cout << "AVL Pool is using " << AvlNode::used() << "% of available nodes\n";
    cout << "JSON Pool is using " << JSON::json_pool_used() << "% of available nodes\n";
/*    
    cout << "test of AVL trees\n";
    AvlNode *tree = null;
    
    tree = AvlNode::avlInsertKey(tree, 5, null);
    tree = AvlNode::avlInsertKey(tree, 1, null);
    tree = AvlNode::avlInsertKey(tree, 6, null);
    tree = AvlNode::avlInsertKey(tree, 2, null);
    cout << "tree size = " << AvlNode::avlSize(tree) << "\n";
    AvlNode::avlPrintKeys(tree);

    HashTable table;
    table.insert_key((const unsigned char *)"one", 11);
    table.insert_key((const unsigned char *)"three", 33);
    table.insert_key((const unsigned char *)"two", 22);
    table.insert_key((const unsigned char *)"hello", 55);
    table.insert_key((const unsigned char *)"mum", 66);
    table.insert_key((const unsigned char *)"brave", 77);
    table.insert_key((const unsigned char *)"world", 88);
    table.print();
*/   
    return 0;
}