#ifndef _WOT_NODE_POOL
#define _WOT_NODE_POOL

#define WOT_NODE_POOL_SIZE 70

typedef uint8_t NPIndex;

// wot_node_pool_t must be large enough to hold JSON and AvlNode
// so we duplicate structure of AvlNode and JSON to ensure that
// this is true for 8 bit MCU's, 32 bit MCUs and 64 bit computers 

union wot_node_pool_t {
    struct {
        char bytes[4];
        void *p;
    };
    struct {
        uint16_t taglen;
        union {
            float x;
            void *q;
        } bar;
    };
};

class WotNodePool
{
    public:
        WotNodePool();
        wot_node_pool_t wot_pool[WOT_NODE_POOL_SIZE];
        unsigned int wot_pool_size;

        void initialise_node_pool(wot_node_pool_t *buffer, unsigned int size);
        unsigned int size();
        float used();
        void *allocate_node();
        wot_node_pool_t *get_pool();
};
#endif