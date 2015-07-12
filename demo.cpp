/* demo code for Web of Things Framework for Arduino */

#include <Arduino.h>
#include "NodePool.h"
#include "AvlNode.h"
#include "HashTable.h"
#include "JSON.h"
#include "WebThings.h"

void setup(Thing *thing, HashTable *table)
{
}

int main ()
{
    WebThings wot;
    
    PRINTLN("started server\n");
    PRINT("size of thing is " );
    PRINTLN(sizeof(Thing));
        
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
        
    wot.thing("agent12", agent_model, setup);
    wot.thing("door12", door_model, setup);
    wot.thing("light12", light_model, setup);
    
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

    const char *test =  "[ \"hello\", [null]]"; //"{\"a\":true}"; "[true]";
    
    PRINTLN("\ntesting JSON parser");
    JSON *json = JSON::parse(test, &table);
    
    PRINTLN("\nparsing complete");
    
    if (json)
        json->print();
    else
        PRINT("undefined");
        
    PRINT("\n");
    
    PRINT("Node Pool is using ");
    PRINT(wot.used());
    PRINTLN("% of available nodes");
    PRINTLN("");
 
    return 0;
}