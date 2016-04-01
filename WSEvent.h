#ifndef _WS_EVENT
#define _WS_EVENT

/*
Event queue support to provide non-overlapping software interrupts for a programming model similar to Web page scripts.

Interrupt service handlers should disable the interrupt, and push an event onto the queue and then re-enable the interrupt. The loop() function should call dispatch() to handle the events from the queue
*/

#define EVENT_QUEUE_LENGTH  6

// add additional event names this enum and update WSEvent.cpp to match
enum Event_t { Network_Readable_Event_t };

typedef void (*Event_hander_t)(void *data);

typedef struct {
    Event_t event;
    void *data;
} EventQueueEntry;

class EventQueue
{
    private:
        int begin;
        int count;
        EventQueueEntry queue[EVENT_QUEUE_LENGTH];
        EventQueueEntry *dequeue();
            
    public:
        EventQueue();
        boolean is_empty();
        int get_size();
        boolean enqueue(Event_t event, void *data);
        void dispatch();
        void set_handler(Event_t event, Event_hander_t handler);
};

#endif
