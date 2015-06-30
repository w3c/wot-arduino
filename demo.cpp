/* demo code for Web of Things Framework for Arduino */

#include <iostream>

using namespace std;

#include "WebThings.h"
#include "MessageCoder.h"

int main ()
{
    WebThings wot;
    
    cout << "started server\n\n";
    
    string agent_model =
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

    string door_model =
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
    
    string light_model =
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
    
    MessageCoder coder;
    
    coder.test();
    
    return 0;
}