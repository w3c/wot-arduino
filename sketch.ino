
#include <avr/pgmspace.h>
#include <Arduino.h>
#include <NodePool.h>
#include <AvlNode.h>
#include <Names.h>
#include <JSON.h>
#include <MessageCoder.h>
#include <WiznetTCP.h>
#include <WSEvent.h>
#include <WebThings.h>
#include <Transport.h>

#define null 0

// an example of a function used to initialise a Thing's implementation
// here you should set event observers, initialise property values and
// binding the thing's actions to the functions you provide for them.
// note that this is your last chance to look up properties by name!

#if 0
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
#endif

//Names names; // to pull in hash table
WebThings wot; // sets up node pool
Transport transport; // TCP client/server
EventQueue event_queue; // sets up event queue

Thing *test1;

void size()
{
    Serial.print(F("Node Pool is using "));
    Serial.print(wot.used());
    Serial.println(F(" nodes"));
}

void setup_test(Thing *thing, Names *names)
{
    test1 = thing;

    Symbol foo = names->symbol(F("foo"));
    Symbol age = names->symbol(F("age"));
    names->print();
    
    size();
    thing->print();
    
    Serial.println(F("set foo to \"hello\""));
    thing->set_property(foo, JSON::new_string(F("hello")));
    
    size();
    thing->print();
    
    Serial.println("set foo to \"world\"");
    thing->set_property(foo, JSON::new_string(F("world")));
    
    size();
    thing->print();
    
    //WebThings::collect_garbage();
    
    //size();

    //thing->print();
    
    Serial.println(F("update property foo to an object with age=42"));
    JSON *obj = JSON::new_object();
    obj->insert_property(age, JSON::new_unsigned(42));
    thing->set_property(foo, obj);

    WebThings::collect_garbage();
    
    size();
    thing->print();
    
    Serial.println(F("update property foo to true"));
    thing->set_property(foo, JSON::new_boolean(true));
    
    size();
    thing->print();

    WebThings::collect_garbage();
    
    size();
    thing->print();
}

void setup() {
    Serial.begin(19200);
    transport.start();
        
 #define TEST_MODEL \
      "{\"properties\": {\"pressure\": \"bar\"}}"
      
//    wot.thing("test1", F(TEST_MODEL), setup_test);
}

void loop() {
  // put your main code here, to run repeatedly:
  
    event_queue.dispatch(); // queued by interrupt services routines
    transport.serve();
}

