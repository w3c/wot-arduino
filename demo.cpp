/* demo code for Web of Things Framework for Arduino */

#include <Arduino.h>
#include "AvlNode.h"
#include "HashTable.h"
#include "JSON.h"
#include "WebThings.h"

int main ()
{
    WebThings wot;
    
    PRINTLN("started server\n");
    
    const char *agent_model PROGMEM = 
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

    const char *door_model PROGMEM =
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
    
    const char *light_model PROGMEM =
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
    
    cout << "test of AVL trees\n";
    AvlIndex tree = 0;
    
    tree = AvlNode::insert_key(tree, 5, null);
    tree = AvlNode::insert_key(tree, 1, null);
    tree = AvlNode::insert_key(tree, 6, null);
    tree = AvlNode::insert_key(tree, 2, null);
    cout << "tree size = " << AvlNode::get_size(tree) << "\n";
    AvlNode::print_keys(tree);
   
    HashTable table;
    table.insert_key((const unsigned char *)"one", 11);
    table.insert_key((const unsigned char *)"three", 33);
    table.insert_key((const unsigned char *)"two", 22);
    table.insert_key((const unsigned char *)"hello", 55);
    table.insert_key((const unsigned char *)"mum", 66);
    table.insert_key((const unsigned char *)"brave", 77);
    table.insert_key((const unsigned char *)"world", 88);
    table.print();

    const char *test = "{\"a\":true}";  //[true]"; //"[ \"hello\", [null]]";
    
    PRINTLN("\ntesting JSON parser");
    JSON *json = JSON::parse(test);
    
    PRINTLN("\nparsing complete");
    
    if (json)
        json->print();
    else
        PRINT("undefined");
        
    PRINT("\n");
    
    PRINT("AVL Pool is using ");
    PRINT(AvlNode::used());
    PRINTLN("% of available nodes");
    
    PRINT("JSON Pool is using ");
    PRINT(JSON::json_pool_used());
    PRINTLN("% of available nodes");
 
    return 0;
}