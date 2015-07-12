#ifndef _WOT_NODE_POOL
#define _WOT_NODE_POOL

#define WOT_NODE_POOL_SIZE 70

typedef uint8_t NPIndex;

typedef struct {
    char byte[4];
    void *pointer;
} wot_node_pool_t;

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