// Web of Things transport layer

#ifndef _WOTF_TRANSPORT
#define _WOTF_TRANSPORT

class Transport
{
    private:
        WiznetTCP tcp;
            
    public:
        void start();
        void stop();
        void serve();
};

#endif