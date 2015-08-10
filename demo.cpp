/* demo code for Web of Things Framework for Arduino */

#include <Arduino.h>
#include "NodePool.h"
#include "AvlNode.h"
#include "Names.h"
#include "JSON.h"
#include "WebThings.h"

// an example of a function used to initialise a Thing's implementation
// here you should set event observers, initialise property values and
// binding the thing's actions to the functions you provide for them.
// note that this is your last chance to look up properties by name!

void setup_agent(Thing *thing, Names *names)
{
    // CoreThing *door = thing->get_property(names->get_symbol("door"));
}

void setup_door(Thing *thing, Names *names)
{
}

void setup_light(Thing *thing, Names *names)
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
        
    wot.thing("agent12", agent_model, setup_agent);
    wot.thing("door12", door_model, setup_door);
    wot.thing("light12", light_model, setup_light);
    
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
   
    Names names;
    names.insert_key((const unsigned char *)"one", 11);
    names.insert_key((const unsigned char *)"three", 33);
    names.insert_key((const unsigned char *)"two", 22);
    names.insert_key((const unsigned char *)"hello", 55);
    names.insert_key((const unsigned char *)"mum", 66);
    names.insert_key((const unsigned char *)"brave", 77);
    names.insert_key((const unsigned char *)"world", 88);
    names.print();

    const char *test =  "[ \"hello\", [null]]"; //"{\"a\":true}"; "[true]";
    
    PRINTLN("\ntesting JSON parser");
    JSON *json = JSON::parse(test, &names);
    
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