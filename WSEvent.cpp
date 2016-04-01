// event queue support

#include <Arduino.h>
#include "WSEvent.h"

static Event_hander_t network_readable_event_handler;

EventQueue::EventQueue()
{
    begin = count = 0;
    network_readable_event_handler = NULL;
}

boolean EventQueue::is_empty()
{
    return count == 0;
}

int EventQueue::get_size()
{
    return count;
}

boolean EventQueue::enqueue(Event_t event, void *data)
{
    EventQueueEntry *entry;
    int index = begin + count;
    
    if (count >= EVENT_QUEUE_LENGTH) {
        //cout << "** event queue overflow\n";
        // should record error somewhere accessible
        Serial.println("event queue overflow");
        return false;
    }
    
    if (index >= EVENT_QUEUE_LENGTH)
        index -= EVENT_QUEUE_LENGTH;
    entry = queue + index;
    entry->event = event;
    entry->data = data;
    ++count;
    return true;
}

EventQueueEntry *EventQueue::dequeue()
{
    if (count == 0)
        return NULL;
        
    noInterrupts();
    int index = begin;
        
    if (++begin >= EVENT_QUEUE_LENGTH)
        begin = 0;
        
    --count;
    interrupts();
    return queue + index;
}

// called from sketch loop function
void EventQueue::dispatch()
{
    EventQueueEntry *entry;
    
    while  ((entry = dequeue()))
    {
        if (entry->event == Network_Readable_Event_t && network_readable_event_handler)
            (*network_readable_event_handler)(entry->data);
    }
}

void EventQueue::set_handler(Event_t event, Event_hander_t handler)
{
    if (event == Network_Readable_Event_t )
        network_readable_event_handler = handler;
}
